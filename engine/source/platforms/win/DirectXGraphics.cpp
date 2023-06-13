#include "DirectXGraphics.h"
#include "Exceptions.h"
#include <iostream>

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
    // pContext->Draw(3u, 0u);
    // pContext->CreateBuffer();
    namespace wrl = Microsoft::WRL;
    wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
    D3D11_BUFFER_DESC bd = {};
    
}

void DirectXGraphics::CreateDevice() 
{
    LOG("CreateDevice begin")
    DXGI_SWAP_CHAIN_DESC sd = {};
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
    sd.Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    GFX_THROW_INFO(
        D3D11CreateDeviceAndSwapChain(
        nullptr,                    // default adapter
        D3D_DRIVER_TYPE_HARDWARE,   // use hardware graphics driver
        nullptr,                    // no software device
        0u,                         // no flags
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