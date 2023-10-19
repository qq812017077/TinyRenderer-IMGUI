#include "DirectXCubeTexture.h"
#include "DirectXGraphics.h"
#include "Exceptions.h"

namespace TinyEngine::Rendering
{

    void DirectXCubeTexture::CreateResource(DirectXGraphics* pGfx)
    {
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = textures[0]->GetWidth();
        texDesc.Height = textures[0]->GetHeight();
        texDesc.MipLevels = 1;                              // no mip mapping
        texDesc.ArraySize = 6;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;
        texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

        // subresource data
        D3D11_SUBRESOURCE_DATA initData[6];
        for(int i = 0; i < 6; i++)
        {
            initData[i].pSysMem = textures[i]->GetImageData();
            initData[i].SysMemPitch = textures[i]->GetPitch();
            initData[i].SysMemSlicePitch = 0;
        }

        INFOMAN(*pGfx);
        // create texture resource
        GFX_THROW_INFO(pGfx->GetDevice()->CreateTexture2D(&texDesc, initData, &pTexture));

        // create the resource view on the texture
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
        srvDesc.TextureCube.MostDetailedMip = 0;
        GFX_THROW_INFO(pGfx->GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

        // create sampler state
        D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };

        GFX_THROW_INFO(pGfx->GetDevice()->CreateSamplerState(&samplerDesc, &pSampler));
    }

    
    void DirectXCubeTexture::BindAsTexture(DirectXGraphics* pGfx, UINT slot)
    {
        pGfx->GetContext()->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
        pGfx->GetContext()->PSGetSamplers(slot, 1, pSampler.GetAddressOf());
    }

    
    void DirectXCubeTexture::LoadFromFiles(std::vector<std::string> paths)
    {
        for(int i = 0; i < 6; i++) textures[i] = Texture::LoadFrom(paths[i]);
    }
}