#include "DirectXGraphics.h"
#include "Exceptions.h"
#include <iostream>
#include "DXUtil.h"

#define LOG(X) std::cout << X << std::endl;
// add ComPtr
namespace wrl = Microsoft::WRL;
HRESULT hr;

DirectXGraphics::DirectXGraphics(HWND &hwnd):
    hWnd(hwnd)
{
    LOG("Directx Graphics  constructor begin")
    CreateDevice();
    CreateRenderTarget();
    LOG("   Directx Graphics  constructor end")
}

DirectXGraphics::~DirectXGraphics()
{
    ClearupDevice();
}

void DirectXGraphics::EndFrame()
{
#ifndef NDEBUG
    infoManager.Set();
#endif
    HRESULT hr;
    if (FAILED(hr = pSwap->Present(1u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
        }
        else
        {
            throw GFX_EXCEPT(hr);   
        }
    }
}

void DirectXGraphics::ClearBuffer(float red, float green, float blue) noexcept
{
    const float color[] = { red, green, blue, 1.0f };
    pContext->ClearRenderTargetView(pTarget.Get(), color);
}

void DirectXGraphics::DrawTestTriangle()
{
    struct Vertex
    {
        float x;
        float y;
    };

    const Vertex vertices[] = {
        {0.0f, 0.5f},
        {0.5f, -0.5f},
        {-0.5f, -0.5f}
    }; 

    // create vertex buffer
    namespace wrl = Microsoft::WRL;
    wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(vertices);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.StructureByteStride = sizeof(Vertex);
    
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;
    GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd ,&pVertexBuffer));
    
    // bind vertex buffer to pipeline
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // set layout
    wrl::ComPtr<ID3D11InputLayout> pInputLayout;
    const D3D11_INPUT_ELEMENT_DESC led[] ={
        {
            "Position", 0u, DXGI_FORMAT_R32G32_FLOAT, 
            0u, 0u, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0u
        }
    };
    // create vertex shader
    wrl::ComPtr<ID3D11VertexShader> pVertexShader;
    wrl::ComPtr<ID3DBlob> pBlob;

    GFX_THROW_INFO(CreateShaderFromFile(L"shaders/VertexShader.cso", L"shaders/VertexShader.hlsl","main", "vs_5_0", &pBlob));
    GFX_THROW_INFO(pDevice->CreateInputLayout(led, (UINT)std::size(led), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));
    GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
    
    
    // create pixel shader
    wrl::ComPtr<ID3D11PixelShader> pPixelShader;
    GFX_THROW_INFO(CreateShaderFromFile(L"shaders/PixelShader.cso", L"shaders/PixelShader.hlsl","main", "ps_5_0", &pBlob));
    GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

    // bind input layout to pipeline
    pContext->IASetInputLayout(pInputLayout.Get());
    // bind pixel and vertex shader to pipeline
    pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
    pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
    // bind render target
    pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = 1280;
    vp.Height = 720;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pContext->RSSetViewports(1u, &vp);


    GFX_THROW_INFO_ONLY(pContext->Draw((UINT)std::size(vertices), 0u));
}

void DirectXGraphics::CreateDevice() 
{
    LOG("CreateDevice begin")
    DXGI_SWAP_CHAIN_DESC sd = {};
    UINT creationFlags = 0u;

    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2u;
    sd.BufferDesc.Width = 0u;
    sd.BufferDesc.Height = 0u;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0u;
    sd.BufferDesc.RefreshRate.Denominator = 0u;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1u;
    sd.SampleDesc.Quality = 0u; // (HWND)1234567;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
#ifndef NDEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    GFX_THROW_INFO(
        D3D11CreateDeviceAndSwapChain(
        nullptr,                    // default adapter
        D3D_DRIVER_TYPE_HARDWARE,   // use hardware graphics driver
        nullptr,                    // no software device
        creationFlags,              // no flags
        nullptr,                    // default feature level array
        0u,                         // default feature level array size
        D3D11_SDK_VERSION,          // SDK version
        &sd,                        // swap chain description
        &pSwap,                     // swap chain
        &pDevice,                   // device
        nullptr,                    // supported feature level
        &pContext                   // device context
    ));

    LOG("   CreateDevice end")
}
void DirectXGraphics::CreateRenderTarget()
{
    LOG("CreateRenderTarget begin")
    wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
    GFX_THROW_INFO(pSwap->GetBuffer(0u, __uuidof(ID3D11Texture2D), &pBackBuffer));
    GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));
    LOG("   CreateRenderTarget end")
}
void DirectXGraphics::ClearupDevice()
{
    
}

void DirectXGraphics::ClearupRenderTarget()
{
    
}