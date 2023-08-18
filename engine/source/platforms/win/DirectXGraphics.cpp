#include "DirectXGraphics.h"
#include "Exceptions.h"
#include <iostream>
#include "DXUtil.h"
#include "HLSLShader.h"
#include "MaterialManager.h"
#include "Texture.h"
#define LOG(X) std::cout << X << std::endl;
// add ComPtr
namespace wrl = Microsoft::WRL;
HRESULT hr;

/****************************************************************************************/
/*                                      Public Part                                     */
/****************************************************************************************/

DirectXGraphics::DirectXGraphics(HWND &hwnd):
    hWnd(hwnd)
{
    LOG("Directx Graphics  constructor begin")
    CreateDevice();
    CreateRenderTarget();
    OnResize(1280, 720);
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

void DirectXGraphics::DrawTestTriangle(float angle)
{
    struct ConstantBuffer
    {
        struct Transform {
            float emelement[4][4];
        };
        Transform rotate;
        Transform scale;
    };

    struct Vertex
    {
        struct
        {
            float x;
            float y;
        } pos;
        struct 
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        } color;
    };

    const Vertex vertices[] = {
        { 0.0f, 0.5f, 255u, 0u, 0u, 255u },
        { 0.5f, -0.5f, 0u, 255u, 0u, 255u },
        { -0.5f, -0.5f, 0u, 0u, 255u, 255u },
        { -0.3f, 0.3f, 0u, 0u, 255u, 255u},
        { 0.3f, 0.3f, 0u, 255u, 0u, 255u },
        { 0.0f, -0.8f, 255u, 0u, 0u, 255u },
    };

    const unsigned short indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 4, 1,
        2, 1, 5
    };
    const ConstantBuffer cb ={
        {
            std::cos(angle),    std::sin(angle),    0.0f,   0.0f,
            -std::sin(angle),   std::cos(angle),    0.0f,   0.0f,
            0.0f,               0.0f,               1.f,    0.0f,
            0.0f,               0.0f,               0.0f,   1.f
        },
        {
            1.f,   0.0f,    0.0f,   0.0f,
            0.0f,   1.0f,    0.0f,   0.0f,
            0.0f,   0.0f,    1.f,    0.0f,
            0.0f,   0.0f,    0.0f,   1.f
        }
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
    
    // create index buffer
    wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
    D3D11_BUFFER_DESC ibd = {};
    ibd.ByteWidth = sizeof(indices);
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.StructureByteStride = sizeof(unsigned short);

    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

    // bind vertex buffer to pipeline
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

    // bind index buffer to pipeline
    pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

    // set primitive topology
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

    // set layout
    wrl::ComPtr<ID3D11InputLayout> pInputLayout;
    const D3D11_INPUT_ELEMENT_DESC led[] ={
        {
            "Position", 0u, DXGI_FORMAT_R32G32_FLOAT, 
            0u, 0u, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0u
        },
        {
            "Color", 0u, DXGI_FORMAT_R8G8B8A8_UNORM,
            0u, 8u, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0u
        }
    };

    // create vertex shader
    wrl::ComPtr<ID3D11VertexShader> pVertexShader;
    wrl::ComPtr<ID3DBlob> pBlob;

    GFX_THROW_INFO(CreateShaderFromFile(L"shaders/VertexShader.cso", L"shaders/VertexShader.hlsl","main", "vs_5_0", &pBlob));
    GFX_THROW_INFO(pDevice->CreateInputLayout(led, (UINT)std::size(led), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));
    GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

    // create constant buffer
    wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
    D3D11_BUFFER_DESC cbd = {};
    cbd.ByteWidth = sizeof(cb);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;
    GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

    // bind constant buffer to vertex shader
    pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());   

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

    // GFX_THROW_INFO_ONLY(pContext->Draw((UINT)std::size(vertices), 0u));
    GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u));
}

/***
 * @brief Draw all game objects
 *  1.  Update PerFrameConstantBuffer( view matrix, projection matrix, camera position, light position, light color, light intensity)
 *  2.  Update PerMaterialConstantBuffer(material color, material texture, material sampler)
 *  3.  Update PerObjectConstantBuffer(world matrix)
*/
void DirectXGraphics::DrawAll()
{
    //1. update per frame constant buffer
    UpdateCBuffer(pFrameConstantBuffer, FrameUniformBufferManager::GetFrameBuffer());
    BindCBuffer(FrameCBufSlot, pFrameConstantBuffer);

    //2. update per material constant buffer
    for(auto & pair : MaterialManager::GetMatRendererPair())
    {
        auto mat = pair.first;
        auto& renderers = pair.second;
        LoadMaterial(*mat);

        for (auto& rendererwapper : renderers)
        {
            auto& renderer = rendererwapper.get();
            UpdateCBuffer(pObjectConstantBuffer, renderer.GetObjBufferData());
            BindCBuffer(ObjectCBufSlot, pObjectConstantBuffer);

            // bind vertex buffer and index buffer
            auto & mesh = renderer.GetMesh();
            // bind input layout
            mat->GetVertexShader()->SetInputLayout(mesh);

            // BindMesh();
            const UINT stride = mesh.GetVertexStride();
            const UINT offset = 0u;
            GFX_THROW_INFO(CreateVertexBuffer(pDevice, mesh.GetVertexBufferAddress(), mesh.GetVertexBufferSize(), mesh.GetVertexStride(), &pVertexBuffer));
            GFX_THROW_INFO(CreateIndexBuffer(pDevice, mesh.GetIndexBufferAddress(), mesh.GetIndexBufferSize(), mesh.GetIndexStride(), &pIndexBuffer));
            
            pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
            // bind index buffer to pipeline
            pContext->IASetIndexBuffer(pIndexBuffer.Get(), GetIndexDataFormat(mesh.GetIndexStride()), 0u);

            // set primitive topology
            pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            
            // bind render target
            pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

            // GFX_THROW_INFO_ONLY(pContext->Draw((UINT)std::size(vertices), 0u));
            GFX_THROW_INFO_ONLY(pContext->DrawIndexed(mesh.GetIndexCount(), 0u, 0u));
        }
    }
}

/****************************************************************************************/
/*                                      Event  Part                                     */
/****************************************************************************************/
void DirectXGraphics::OnResize(int width, int height)
{
    // configure viewport

    D3D11_VIEWPORT vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pContext->RSSetViewports(1u, &vp);
}


/***********************************************************************************************************/
/*                                            Protected Part                                               */
/***********************************************************************************************************/
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


std::shared_ptr<VertexShader> DirectXGraphics::CreateVertexShader(const std::string& path)
{
    std::shared_ptr<HLSLVertexShader> pShader(new HLSLVertexShader(*this, std::wstring(path.begin(), path.end())));
    return pShader;
}

std::shared_ptr<PixelShader> DirectXGraphics::CreatePixelShader(const std::string& path)
{   
    std::shared_ptr<HLSLPixelShader> pShader(new HLSLPixelShader(*this, std::wstring(path.begin(), path.end())));
    return pShader;
}

/****************************************************************************************/
/*                                Material Operation                                    */
/****************************************************************************************/

/**
 * @brief Load material to GPU
 *          bind vertex shader, pixel shader, constant buffer, texture to pipeline
 * 
 * @param material 
 */
void DirectXGraphics::LoadMaterial(Material & material)
{
    Graphics::LoadMaterial(material);

    HLSLVertexShader* pVertexShader = dynamic_cast<HLSLVertexShader*>(material.GetVertexShader().get());
    HLSLPixelShader* pPixelShader = dynamic_cast<HLSLPixelShader*>(material.GetPixelShader().get());
    pContext->VSSetShader(pVertexShader->Get(), nullptr, 0u);
    pContext->PSSetShader(pPixelShader->Get(), nullptr, 0u);


    auto vsConstantBuffer = pVertexShader->GetConstantBuffer();
    auto psConstantBuffer = pPixelShader->GetConstantBuffer();

    auto vsUniformBuffer = material.GetVertexUniformBuffer();
    auto psUniformBuffer = material.GetPixelUniformBuffer();
    if(vsUniformBuffer.bytesize() != 0)
    {
        UpdateCBuffer(vsConstantBuffer, vsUniformBuffer);
        BindCBuffer(MaterialCBufSlot, vsConstantBuffer, Graphics::EBindType::ToVS);
    }
    if(psUniformBuffer.bytesize() != 0) 
    {
        UpdateCBuffer(psConstantBuffer, psUniformBuffer);
        BindCBuffer(MaterialCBufSlot, psConstantBuffer, Graphics::EBindType::ToPS);
    }
    
    // TODO: bind texture to shader
    for(int i = 0, imax = pVertexShader->GetTexCount(); i < imax; i++)
    {
        TextureInfo texInfo = pVertexShader->GetTexInfoByIndex(i);
        auto pTex = material.GetTexturePtr(texInfo.name);
        
        Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2D;
        bool success = pVertexShader->CreateTexture2D(pTex, &pTexture2D);
        if(!success) continue;

        SamplerInfo samplerInfo = pVertexShader->GetSamplerInfoByName(material.GetSamplerNameByTexName(texInfo.name), (int)texInfo.slot);
        
        Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
        success = pVertexShader->CreateSampler(pTex, &pSamplerState);
        if(!success) continue;
        
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
        pDevice->CreateShaderResourceView(pTexture2D.Get(), nullptr, &pShaderResourceView );
        pContext->VSSetShaderResources((UINT)(texInfo.slot), 1u, pShaderResourceView.GetAddressOf());
        pContext->PSSetSamplers((UINT)(samplerInfo.slot), 1u, pSamplerState.GetAddressOf());
    }

    // // Create ID3D11 Texture to shader
    for(int i = 0, imax = pPixelShader->GetTexCount(); i < imax; i++)
    {
        TextureInfo texInfo = pPixelShader->GetTexInfoByIndex(i);
        auto pTex = material.GetTexturePtr(texInfo.name);
        
        Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2D;
        bool success = pPixelShader->CreateTexture2D(pTex, &pTexture2D);
        if(!success) continue;
        
        SamplerInfo samplerInfo = pPixelShader->GetSamplerInfoByName(material.GetSamplerNameByTexName(texInfo.name), (int)texInfo.slot);
        Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
        success = pPixelShader->CreateSampler(pTex, &pSamplerState);
        if(!success) continue;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
        pDevice->CreateShaderResourceView(pTexture2D.Get(), nullptr, &pShaderResourceView );
        pContext->PSSetShaderResources((UINT)(texInfo.slot), 1u, pShaderResourceView.GetAddressOf());
        pContext->PSSetSamplers((UINT)(samplerInfo.slot), 1u, pSamplerState.GetAddressOf());
    }
}
/****************************************************************************************/
/*                                Constant Buffer Operation                             */
/****************************************************************************************/
void DirectXGraphics::UpdateCBuffer(wrl::ComPtr<ID3D11Buffer>& targetBuf, UniformBuffer & bufData)
{
    if(bufData.bytesize() == 0)
    {
        // LOG("bufData is empty. " <<__FILE__ << "[" << __LINE__  << "]");
        return ;
    }
    // GFX_THROW_INFO(CreateConstantBuffer(pDevice, bufData.GetAddress(), bufData.bytesize(), &targetBuf));
    if(targetBuf == nullptr)
    {
        GFX_THROW_INFO(CreateConstantBuffer(pDevice, bufData.GetAddress(), bufData.bytesize(), &targetBuf));
    }else
    {
        // update constant buffer by map and unmap
        D3D11_MAPPED_SUBRESOURCE msr;
        pContext->Map(targetBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
        memcpy_s(msr.pData, bufData.bytesize(), bufData.GetAddress(), bufData.bytesize());
        pContext->Unmap(targetBuf.Get(), 0);
        // pContext->UpdateSubresource(targetBuf.Get(), 0, nullptr, bufData.GetAddress(), 0, 0);
    }
}

void DirectXGraphics::BindCBuffer(unsigned int slot, wrl::ComPtr<ID3D11Buffer>& targetBuf, Graphics::EBindType bindType)
{
    if(targetBuf == nullptr)
    {
        // LOG("targetBuf is empty." <<__FILE__ << "[" << __LINE__  << "]");
        return ;
    }
    if(bindType == Graphics::EBindType::ToVS)
        pContext->VSSetConstantBuffers(slot, 1u, targetBuf.GetAddressOf());
    else if(bindType == Graphics::EBindType::ToPS)
        pContext->PSSetConstantBuffers(slot, 1u, targetBuf.GetAddressOf());
    else if(bindType == Graphics::EBindType::ToAll)
    {
        pContext->VSSetConstantBuffers(slot, 1u, targetBuf.GetAddressOf());
        pContext->PSSetConstantBuffers(slot, 1u, targetBuf.GetAddressOf());
    }
}

/****************************************************************************************/
/*                                Texture Operation                                     */
/****************************************************************************************/
void DirectXGraphics::UpdateTexture(wrl::ComPtr<ID3D11ShaderResourceView>& pTextureView, const Texture& texture)
{
    if(pTextureView == nullptr)
    {
        LOG("pTextureView is empty." <<__FILE__ << "[" << __LINE__  << "]");
        return ;
    }
    // //
    // ID3D10Resource* pResource = nullptr;
    // pTextureView->GetResource(&pResource);
    // ID3D11Texture2D* pTexture = nullptr;
    // pResource->QueryInterface(&pTexture);
    // D3D11_TEXTURE2D_DESC td;
    // pTexture->GetDesc(&td);
    // if(td.Width != texture.GetWidth() || td.Height != texture.GetHeight())
    // {
    //     LOG("texture size is not match." <<__FILE__ << "[" << __LINE__  << "]");
    //     return ;
    // }
    // // update texture by map and unmap
    // D3D11_MAPPED_SUBRESOURCE msr;
    // pContext->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    // memcpy_s(msr.pData, texture.GetWidth() * texture.GetHeight() * 4, texture.GetAddress(), texture.GetWidth() * texture.GetHeight() * 4);
    // pContext->Unmap(pTexture, 0);
    
}

void DirectXGraphics::BindTexture(unsigned int slot, wrl::ComPtr<ID3D11ShaderResourceView>& pTextureView, Graphics::EBindType bindType)
{
    if(pTextureView == nullptr)
    {
        LOG("targetBuf is empty." <<__FILE__ << "[" << __LINE__  << "]");
        return ;
    }

    if(bindType == Graphics::EBindType::ToVS)
        pContext->VSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
    else if(bindType == Graphics::EBindType::ToPS)
        pContext->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
    else if(bindType == Graphics::EBindType::ToAll)
    {
        pContext->VSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
        pContext->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
    }
}
