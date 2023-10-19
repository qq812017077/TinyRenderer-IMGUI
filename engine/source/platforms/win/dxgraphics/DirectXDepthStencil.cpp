#include "DirectXDepthStencil.h"
#include "DirectXGraphics.h"
#include "Exceptions.h"
#include "core/EngineException.h"

namespace wrl = Microsoft::WRL;
namespace TinyEngine
{

    DXGI_FORMAT MapUsageTypeless(DirectXDepthStencil::Usage usage)
    {
        switch(usage)
        {
        case DirectXDepthStencil::Usage::DepthStencil:
            return DXGI_FORMAT_R24G8_TYPELESS;
        case DirectXDepthStencil::Usage::ShadowDepth:
            return DXGI_FORMAT_R32_TYPELESS;
        }

        THROW_ENGINE_EXCEPTION("unknown usage");
    }

    DXGI_FORMAT MapUsageDepthStencil(DirectXDepthStencil::Usage usage)
    {
        switch(usage)
        {
        case DirectXDepthStencil::Usage::DepthStencil:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case DirectXDepthStencil::Usage::ShadowDepth:
            return DXGI_FORMAT_D32_FLOAT;
        }

        THROW_ENGINE_EXCEPTION("unknown usage");
    }

    DXGI_FORMAT MapUsageShaderResource(DirectXDepthStencil::Usage usage)
    {
        switch(usage)
        {
        case DirectXDepthStencil::Usage::DepthStencil:
            return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        case DirectXDepthStencil::Usage::ShadowDepth:
            return DXGI_FORMAT_R32_FLOAT;
        }
        THROW_ENGINE_EXCEPTION("unknown usage");
    }

    DirectXDepthStencil::DirectXDepthStencil(DirectXGraphics* pGfx, unsigned int width, unsigned int height, Usage usage)
    : DepthStencil(pGfx, width, height)
    {
        bool canBindShaderInput = usage == Usage::ShadowDepth;
        
        wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
        D3D11_TEXTURE2D_DESC depthStencilDesc = {};
        depthStencilDesc.Width = width;
        depthStencilDesc.Height = height;
        depthStencilDesc.MipLevels = 1u;
        depthStencilDesc.ArraySize = 1u;
        depthStencilDesc.Format = MapUsageTypeless(usage);
        depthStencilDesc.SampleDesc.Count = 1u;
        depthStencilDesc.SampleDesc.Quality = 0u;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL |  (canBindShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0);
        depthStencilDesc.CPUAccessFlags = 0u;

        INFOMAN(*pGfx);
        GFX_THROW_INFO(pGfx->GetDevice()->CreateTexture2D(&depthStencilDesc, nullptr, &pDepthStencil));
        
        if(canBindShaderInput)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = MapUsageShaderResource( usage );
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = 1;
            GFX_THROW_INFO(pGfx->GetDevice()->CreateShaderResourceView(pDepthStencil.Get(), &srvDesc, &pTextureView));

            D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
            samplerDesc.BorderColor[0] = 1.0f;
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;

            GFX_THROW_INFO(pGfx->GetDevice()->CreateSamplerState(&samplerDesc, &pSamplerState));
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
        depthStencilViewDesc.Format = MapUsageDepthStencil(usage);
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0u;
        depthStencilViewDesc.Flags = 0u;

        GFX_THROW_INFO(pGfx->GetDevice()->CreateDepthStencilView(pDepthStencil.Get(), &depthStencilViewDesc, &pDepthStencilView));
    }

    void DirectXDepthStencil::Clear(Graphics* pGfx)
    {
        internalClear(reinterpret_cast<DirectXGraphics*>(pGfx));
    }
    
    void DirectXDepthStencil::BindAsTexture(Graphics* pGfx, unsigned int slot)
    {
        ID3D11Resource * pResource = nullptr;
        pTextureView->GetResource(&pResource);
        // get width and height
        D3D11_TEXTURE2D_DESC desc;
        reinterpret_cast<ID3D11Texture2D*>(pResource)->GetDesc(&desc);
        pResource->Release();
        
        auto dxGfx = reinterpret_cast<DirectXGraphics*>(pGfx);
        dxGfx->BindTexture(slot, pTextureView, Graphics::EBindType::ToPS);
        dxGfx->BindSampler(slot, pSamplerState, Graphics::EBindType::ToPS);
    }
    void DirectXDepthStencil::internalClear(DirectXGraphics* pGfx)
    {
        pGfx->GetContext()->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
    }


    std::shared_ptr<DirectXDepthStencil> DirectXDepthStencil::Create(Graphics* pGfx, TinyEngine::Graph::ResourceDesc desc)
    {
        auto dxGfx = dynamic_cast<DirectXGraphics*>(pGfx);
        if(dxGfx == nullptr) THROW_ENGINE_EXCEPTION("graphics type mismatch");

        switch (desc.type)
        {
        case TinyEngine::Graph::ResourceType::DepthStencil:
            return std::make_shared<DirectXDepthStencil>(dxGfx, dxGfx->GetWidth(), dxGfx->GetHeight());
        case TinyEngine::Graph::ResourceType::ShadowMap:
            return std::make_shared<DirectXDepthStencil>(dxGfx, dxGfx->GetWidth(), dxGfx->GetHeight() , Usage::ShadowDepth);
        default:
            break;
        }
        THROW_ENGINE_EXCEPTION("cannot create render target from resource desc");
    }
}