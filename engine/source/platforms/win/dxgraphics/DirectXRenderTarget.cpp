#include "DirectXRenderTarget.h"
#include "DirectXGraphics.h"
#include "Exceptions.h"
#include "DirectXDepthStencil.h"
#include "graph/RenderResource.h"
namespace wrl = Microsoft::WRL;
namespace TinyEngine
{
    
    DirectXRenderTarget::DirectXRenderTarget(DirectXGraphics* pGfx, ID3D11Texture2D* pTexture):
        RenderTarget(pGfx, -1, -1)
    {
        INFOMAN(*pGfx);
        
        D3D11_TEXTURE2D_DESC textureDesc;
		pTexture->GetDesc( &textureDesc );
		m_width = textureDesc.Width;
		m_height = textureDesc.Height;
        GFX_THROW_INFO(pGfx->GetDevice()->CreateRenderTargetView(pTexture, nullptr, &pTargetView));
    }


    DirectXRenderTarget::DirectXRenderTarget(DirectXGraphics* pGfx, unsigned int width, unsigned int height):
        RenderTarget(pGfx, width, height)
    {
        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 1u;
        textureDesc.ArraySize = 1u;
        textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        textureDesc.SampleDesc.Count = 1u;
        textureDesc.SampleDesc.Quality = 0u;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.CPUAccessFlags = 0u;
        // textureDesc.MiscFlags = 0u;
        
        wrl::ComPtr<ID3D11Texture2D> pTexture;
        INFOMAN(*pGfx);
        GFX_THROW_INFO(pGfx->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture));
    
        // create the resource view on the texture
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1u;
        srvDesc.Texture2D.MostDetailedMip = 0u;
        GFX_THROW_INFO(pGfx->GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

        // create the target view on the texture
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = textureDesc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        // rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
        rtvDesc.Texture2D.MipSlice = 0u;
        GFX_THROW_INFO(pGfx->GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, &pTargetView));
    }


    
    void DirectXRenderTarget::Clear(Graphics* pGfx)
    {
        internalClear(static_cast<DirectXGraphics*>(pGfx));
    }
    void DirectXRenderTarget::BindAsTexture(Graphics* pGfx, unsigned int slot)
    {
        reinterpret_cast<DirectXGraphics*>(pGfx)->BindTexture(slot, pTextureView, Graphics::EBindType::ToPS);
    }
    void DirectXRenderTarget::internalClear(DirectXGraphics* pGfx)
    {
        const float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        pGfx->GetContext()->ClearRenderTargetView(pTargetView.Get(), color);
    }

    std::shared_ptr<DirectXRenderTarget> DirectXRenderTarget::Create(Graphics* pGfx, TinyEngine::Graph::ResourceDesc desc)
    {
        auto dxGfx = dynamic_cast<DirectXGraphics*>(pGfx);
        if(dxGfx == nullptr) THROW_ENGINE_EXCEPTION("graphics type mismatch");
        if(desc.type == TinyEngine::Graph::ResourceType::RenderTarget)
        {
            return std::make_shared<DirectXRenderTarget>(dxGfx, dxGfx->GetWidth(), dxGfx->GetHeight());
        }else if(desc.type == TinyEngine::Graph::ResourceType::SwapChain)
        {
            return std::make_shared<DirectXRenderTarget>(dxGfx, dxGfx->GetBackBuffer().Get());
        }

        THROW_ENGINE_EXCEPTION("cannot create render target from resource desc: type mismatch: ");
        
    }
}