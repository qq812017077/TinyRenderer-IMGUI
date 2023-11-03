#include "DirectXRenderTarget.h"
#include "DirectXGraphics.h"
#include "Exceptions.h"
#include "DirectXDepthStencil.h"
#include "graph/RenderResource.h"
#include "Texture.h"

namespace wrl = Microsoft::WRL;
namespace TinyEngine
{
    static DXGI_FORMAT GetDXFormat(ETextureFormat format)
    {
        switch(format)
        {
            case ETextureFormat::RGBA32: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case ETextureFormat::RFloat: return DXGI_FORMAT_R32_FLOAT;
            case ETextureFormat::RGBAFloat: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }

        THROW_ENGINE_EXCEPTION("unsupported texture format");
    }
    DirectXRenderTarget::DirectXRenderTarget(DirectXGraphics* pGfx, ID3D11Texture2D* pTexture, std::optional<UINT> face, std::optional<UINT> mipmapLevel):
        RenderTarget(pGfx, -1, -1)
    {
        INFOMAN(*pGfx);
        
        D3D11_TEXTURE2D_DESC textureDesc;
		pTexture->GetDesc( &textureDesc );
		m_width = textureDesc.Width;
		m_height = textureDesc.Height;

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = textureDesc.Format;
        if(face.has_value())
        {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2DArray.ArraySize = 1u;
            rtvDesc.Texture2DArray.FirstArraySlice = face.value();
            rtvDesc.Texture2DArray.MipSlice = mipmapLevel.value_or(0);
        }else
        {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
        }

        GFX_THROW_INFO(pGfx->GetDevice()->CreateRenderTargetView(pTexture, &rtvDesc, &pTargetView));
    }


    DirectXRenderTarget::DirectXRenderTarget(DirectXGraphics* pGfx, unsigned int width, unsigned int height, DXGI_FORMAT format):
        RenderTarget(pGfx, width, height)
    {
        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 1u;
        textureDesc.ArraySize = 1u;
        textureDesc.Format = format;
        textureDesc.SampleDesc.Count = 1u;
        textureDesc.SampleDesc.Quality = 0u;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.CPUAccessFlags = 0u;
        
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
        }else if(desc.type == TinyEngine::Graph::ResourceType::InstanceID)
        {
            return std::make_shared<DirectXRenderTarget>(dxGfx, dxGfx->GetWidth(), dxGfx->GetHeight(), DXGI_FORMAT_R32_UINT);
        }

        THROW_ENGINE_EXCEPTION("cannot create render target from resource desc: type mismatch: ");
        
    }

    CubeRenderTexture::CubeRenderTexture(DirectXGraphics* pGfx, TextureResDesc _texDesc): 
        BufferResource(pGfx, _texDesc.width, _texDesc.height)
    {
        INFOMAN(*pGfx);
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = _texDesc.width;
        texDesc.Height = _texDesc.height;
        texDesc.MipLevels = _texDesc.mipLevels;                              // no mip mapping
        texDesc.ArraySize = 6;
        texDesc.Format = GetDXFormat(_texDesc.format);
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        texDesc.CPUAccessFlags = 0;
        texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

        //create the texture resource
        wrl::ComPtr<ID3D11Texture2D> pTexture;

        // create the resource view on the texture
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
        srvDesc.TextureCube.MostDetailedMip = 0;

        if(texDesc.MipLevels != 1) texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        GFX_THROW_INFO(pGfx->GetDevice()->CreateTexture2D(&texDesc, nullptr, &pTexture));
        GFX_THROW_INFO(pGfx->GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

        for(unsigned int mipmapLevel = 0; mipmapLevel < texDesc.MipLevels; mipmapLevel++)
        {
            std::vector<std::unique_ptr<DirectXRenderTarget>> faceBuffers;
            for(unsigned int face = 0 ; face < 6; face++)
            {
                faceBuffers.push_back(std::make_unique<DirectXRenderTarget>(pGfx, pTexture.Get(), face, mipmapLevel));
            }
            renderBuffers.push_back(std::move(faceBuffers));
        }
        // for(unsigned int face = 0 ; face < 6; face++)
        // {
        //     renderBuffers.push_back(std::make_unique<DirectXRenderTarget>(pGfx, pTexture.Get(), face));
        // }
    }
    
    // CubeRenderTexture::CubeRenderTexture(DirectXGraphics* pGfx, unsigned int texSize, int mipmapLevels, DXGI_FORMAT format): BufferResource(pGfx, texSize, texSize)
    // {
    //     INFOMAN(*pGfx);

    //     D3D11_TEXTURE2D_DESC texDesc = {};
    //     texDesc.Width = texSize;
    //     texDesc.Height = texSize;
    //     texDesc.MipLevels = mipmapLevels;                                           // no mip mapping
    //     texDesc.ArraySize = 6;
    //     texDesc.Format = DXGI_FORMAT_R32_FLOAT;
    //     texDesc.SampleDesc.Count = 1;
    //     texDesc.SampleDesc.Quality = 0;
    //     texDesc.Usage = D3D11_USAGE_DEFAULT;
    //     texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    //     texDesc.CPUAccessFlags = 0;
    //     texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    //     //create the texture resource
    //     wrl::ComPtr<ID3D11Texture2D> pTexture;
    //     GFX_THROW_INFO(pGfx->GetDevice()->CreateTexture2D(&texDesc, nullptr, &pTexture));

    //     // create the resource view on the texture
    //     D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    //     srvDesc.Format = texDesc.Format;
    //     srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    //     srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
    //     srvDesc.TextureCube.MostDetailedMip = 0;
    //     GFX_THROW_INFO(pGfx->GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

    //     for(unsigned int face = 0 ; face < 6; face++)
    //     {
    //         renderBuffers.push_back(std::make_unique<DirectXRenderTarget>(pGfx, pTexture.Get(), face));
    //     }
    // }

    void CubeRenderTexture::BindAsTexture(DirectXGraphics* pGfx, UINT slot)
    {
        auto dxGfx = reinterpret_cast<DirectXGraphics*>(pGfx);
        dxGfx->BindTexture(slot, pTextureView, Graphics::EBindType::ToPS);
    }
    DirectXRenderTarget * CubeRenderTexture::GetFaceBuffer(unsigned int face, int mipmapLevel)
    { 
        return renderBuffers[mipmapLevel][face].get(); 
    }

    void CubeRenderTexture::Clear(Graphics* pGfx)
    {
        for(int i = 0, size = renderBuffers.size(); i < size; i++)
        {
            for(int j = 0; j < 6; j++)
                renderBuffers[i][j]->Clear(pGfx);
        }
    }
    
    std::shared_ptr<CubeRenderTexture> CubeRenderTexture::Create(Graphics* pGfx, TinyEngine::Graph::ResourceDesc desc)
    {
        if(desc.type == TinyEngine::Graph::ResourceType::ShadowCubeMap)
        {
            TextureResDesc texDesc;
            texDesc.width = 1024;
            texDesc.height = 1024;
            texDesc.mipLevels = 1;
            texDesc.format = ETextureFormat::RFloat;
            return std::make_shared<CubeRenderTexture>(reinterpret_cast<DirectXGraphics*>(pGfx), texDesc); // TODO: customize size
        }
        THROW_ENGINE_EXCEPTION("cannot create render target from resource desc");
    }
}