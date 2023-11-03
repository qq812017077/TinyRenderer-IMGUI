#include "DirectXCubeTexture.h"
#include "DirectXGraphics.h"
#include "Exceptions.h"
#include "DXUtil.h"


namespace TinyEngine::Rendering
{
    void DirectXCubeTexture::CreateResource(DirectXGraphics* pGfx)
    {
        if(pCubeTex->textures[0] == nullptr)  return ;

        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = pCubeTex->textures[0]->GetWidth();
        texDesc.Height = pCubeTex->textures[0]->GetHeight();
        texDesc.MipLevels = pCubeTex->textures[0]->GetMipMapLevels();                              // no mip mapping
        texDesc.ArraySize = 6;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;
        texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
        
        // create the resource view on the texture
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
        srvDesc.TextureCube.MostDetailedMip = 0;

        INFOMAN(*pGfx);


        std::unique_ptr<D3D11_SUBRESOURCE_DATA[]> initData = nullptr;
        // subresource data
        /**
         * @ref https://github.com/microsoft/DirectXTex/blob/main/DirectXTex/DirectXTexD3D11.cpp
         */
        if(texDesc.MipLevels == 0)
        {
            THROW_ENGINE_EXCEPTION("MipLevels is 0, we don't support this");
        }else 
        {
            initData = std::make_unique<D3D11_SUBRESOURCE_DATA[]>(texDesc.MipLevels * 6);
            if(texDesc.MipLevels > 1)
            {
                texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
                texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
                size_t idx = 0;
                for(int item = 0; item < 6; item++)
                {
                    for (size_t level = 0; level < texDesc.MipLevels; ++level)
                    {
                        initData[idx].pSysMem = pCubeTex->textures[item]->GetMipmapImageData(level);
                        initData[idx].SysMemPitch = pCubeTex->textures[item]->GetMipmapRowPitch(level);
                        initData[idx].SysMemSlicePitch = 0;
                        idx++;
                    }
                }
            }else
            {
                for(int i = 0; i < 6; i++)
                {
                    initData[i].pSysMem = pCubeTex->textures[i]->GetImageData();
                    initData[i].SysMemPitch = pCubeTex->textures[i]->GetRowPitch();
                    initData[i].SysMemSlicePitch = 0;
                }
            }

            Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture = nullptr;
            
            // create texture resource
            GFX_THROW_INFO(pGfx->GetDevice()->CreateTexture2D(&texDesc, initData.get(), &pTexture));
            GFX_THROW_INFO(pGfx->GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));
        }
        
        // create sampler state
        D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
        GFX_THROW_INFO(pGfx->GetDevice()->CreateSamplerState(&samplerDesc, &pSampler));
    }

    
    void DirectXCubeTexture::BindAsTexture(DirectXGraphics* pGfx, UINT slot)
    {
        if(pTextureView == nullptr) return;
        pGfx->GetContext()->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
        pGfx->GetContext()->PSSetSamplers(slot, 1, pSampler.GetAddressOf());
    }

    
    void DirectXCubeTexture::BindCubeMap(std::shared_ptr<CubeTexture> pCubeTex)
    {
        if(this->pCubeTex == pCubeTex) return;
        this->pCubeTex = pCubeTex;

        pTextureView = nullptr;
        pSampler = nullptr;
    }

    void DirectXTexture::CreateResource(DirectXGraphics* pGfx)
    {
        if(pTex == nullptr) return;

        CreateTexture2DView(pGfx->GetDevice(), pGfx->GetContext(), pTex.get(), &pTextureView);

        INFOMAN(*pGfx);
        // create sampler state
        D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };

        samplerDesc.AddressU = GetTextureWrapMode(pTex->GetWrapMode());
        samplerDesc.AddressV = GetTextureWrapMode(pTex->GetWrapMode());
        samplerDesc.AddressW = GetTextureWrapMode(pTex->GetWrapMode());
        GFX_THROW_INFO(pGfx->GetDevice()->CreateSamplerState(&samplerDesc, &pSampler));
    }

    void DirectXTexture::BindAsTexture(DirectXGraphics* pGfx, UINT slot)
    {
        if(pTextureView == nullptr) return;
        pGfx->GetContext()->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
        pGfx->GetContext()->PSSetSamplers(slot, 1, pSampler.GetAddressOf());
    }

    void DirectXTexture::BindTexture(std::shared_ptr<Texture> pTexture)
    {
        if(this->pTex == pTexture) return;
        this->pTex = pTexture;
        // release previous texture
        pTextureView = nullptr;
        pSampler = nullptr;
    }

}