#include "DirectXDepthStencil.h"
#include "DirectXGraphics.h"
#include "Exceptions.h"
#include "core/EngineException.h"
#include "Texture.h"
#include "DXUtil.h"

namespace wrl = Microsoft::WRL;
namespace TinyEngine
{
    DXGI_FORMAT GetTypelessFormat(ETextureFormat format)
    {
        switch(format)
        {
            case ETextureFormat::RGBA32: return DXGI_FORMAT_R8G8B8A8_TYPELESS;
            case ETextureFormat::RFloat: return DXGI_FORMAT_R32_TYPELESS;
            case ETextureFormat::RGBAFloat: return DXGI_FORMAT_R32G32B32A32_TYPELESS;
        }
        THROW_ENGINE_EXCEPTION("unknown usage");
    }
    
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
            samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // 
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;       // if value less than value 's shadow map, pass-> can see

            GFX_THROW_INFO(pGfx->GetDevice()->CreateSamplerState(&samplerDesc, &pSamplerState));
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
        depthStencilViewDesc.Format = MapUsageDepthStencil(usage);
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0u;
        depthStencilViewDesc.Flags = 0u;

        GFX_THROW_INFO(pGfx->GetDevice()->CreateDepthStencilView(pDepthStencil.Get(), &depthStencilViewDesc, &pDepthStencilView));
    }

    
    DirectXDepthStencil::DirectXDepthStencil(DirectXGraphics* pGfx, wrl::ComPtr<ID3D11Texture2D>& pTexture, unsigned int face)
    : DepthStencil(pGfx, -1, -1)
    {
        INFOMAN(*pGfx);
        Usage usage = Usage::ShadowDepth;
        D3D11_TEXTURE2D_DESC textureDesc;
		pTexture->GetDesc( &textureDesc );
		m_width = textureDesc.Width;
		m_height = textureDesc.Height;

        D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
        viewDesc.Format = MapUsageDepthStencil(usage);
        viewDesc.Flags = 0u;
        viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        viewDesc.Texture2D.MipSlice = 0u;
        viewDesc.Texture2DArray.ArraySize = 1u;
        viewDesc.Texture2DArray.FirstArraySlice = face;
        GFX_THROW_INFO(pGfx->GetDevice()->CreateDepthStencilView(pTexture.Get(), &viewDesc, &pDepthStencilView));
    }

    void DirectXDepthStencil::Clear(Graphics* pGfx)
    {
        internalClear(reinterpret_cast<DirectXGraphics*>(pGfx));
    }
    
    void DirectXDepthStencil::BindAsTexture(Graphics* pGfx, unsigned int slot)
    {
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


    DitectXDepthArray::DitectXDepthArray(DirectXGraphics* pGfx, TextureResDesc& texDesc) : DepthStencil(pGfx, texDesc.width, texDesc.height)
    {
        wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
        D3D11_TEXTURE2D_DESC depthStencilDesc = {};
        depthStencilDesc.Width = texDesc.width;
        depthStencilDesc.Height = texDesc.height;
        depthStencilDesc.MipLevels = texDesc.mipLevels;
        depthStencilDesc.ArraySize = texDesc.arraySize;
        depthStencilDesc.Format = GetTypelessFormat(texDesc.format);
        depthStencilDesc.SampleDesc.Count = 1u;
        depthStencilDesc.SampleDesc.Quality = 0u;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;

        depthStencilDesc.BindFlags = GetTextureBindFlag(texDesc.bind);
        depthStencilDesc.CPUAccessFlags = 0u;

        INFOMAN(*pGfx);
        GFX_THROW_INFO(pGfx->GetDevice()->CreateTexture2D(&depthStencilDesc, nullptr, &pDepthStencil));


        if(texDesc.bind & TextureResDesc::BindFlag::ShaderResource)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = GetTextureFormat(texDesc.format);
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.ArraySize = texDesc.arraySize;
            srvDesc.Texture2DArray.FirstArraySlice = 0;
            srvDesc.Texture2DArray.MipLevels = texDesc.mipLevels;
            srvDesc.Texture2DArray.MostDetailedMip = 0;
            GFX_THROW_INFO(pGfx->GetDevice()->CreateShaderResourceView(pDepthStencil.Get(), &srvDesc, &pTextureView));

            D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
            samplerDesc.BorderColor[0] = 1.0f;
            samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // 
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;       // if value less than value 's shadow map, pass-> can see

            GFX_THROW_INFO(pGfx->GetDevice()->CreateSamplerState(&samplerDesc, &pSamplerState));
        }
        
        depthBuffers.clear();
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
        for(unsigned int i = 0 ; i < texDesc.arraySize; ++i)
        {
            depthBuffers.emplace_back(std::make_unique<DirectXDepthStencil>(pGfx, pDepthStencil, i));
        }
        
    }
    void DitectXDepthArray::BindAsTexture(Graphics* pGfx, unsigned int slot)
    {
        auto dxGfx = reinterpret_cast<DirectXGraphics*>(pGfx);
        dxGfx->BindTexture(slot, pTextureView, Graphics::EBindType::ToPS);
        dxGfx->BindSampler(slot, pSamplerState, Graphics::EBindType::ToPS);
    }
    void DitectXDepthArray::Clear(Graphics* pGfx)
    {
        auto dxGfx = reinterpret_cast<DirectXGraphics*>(pGfx);
        for(auto& depthBuffer : depthBuffers)
        {
            depthBuffer->Clear(pGfx);
        }
    }

    std::shared_ptr<DitectXDepthArray> DitectXDepthArray::Create(Graphics* pGfx, TinyEngine::Graph::ResourceDesc desc)
    {
        auto dxGfx = dynamic_cast<DirectXGraphics*>(pGfx);
        if(dxGfx == nullptr) THROW_ENGINE_EXCEPTION("graphics type mismatch");

        switch (desc.type)
        {
            case TinyEngine::Graph::ResourceType::ShadowMap:
                TextureResDesc texDesc;
                texDesc.bind = TextureResDesc::BindFlag::DepthStencil | TextureResDesc::BindFlag::ShaderResource;
                texDesc.width = dxGfx->GetWidth();
                texDesc.height = dxGfx->GetHeight();
                texDesc.arraySize = 4;
                texDesc.format = ETextureFormat::RFloat;
                texDesc.wrapMode = EWrapMode::Border;
                return std::make_shared<DitectXDepthArray>(dxGfx, texDesc);
        }
        THROW_ENGINE_EXCEPTION("cannot create render target from resource desc");
        return nullptr;
    }
    
}