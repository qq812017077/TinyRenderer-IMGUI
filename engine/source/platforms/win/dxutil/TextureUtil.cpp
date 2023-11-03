#include "TextureUtil.h"
#include "dxgraph/DirectXRenderGraph.h"
#include "DirectXGraphics.h"
#include "Texture.h"
#include "core/math/EngineMath.h"
#include "HLSLShaderHelper.h"
#include "dxgraphics/DirectXRenderTarget.h"
#include "dxgraphics/DirectXDepthStencil.h"
#include "DirectXCubeTexture.h"
#include "Material.h"
#include "RenderEntity.h"
#include "geometry/Primitive.h"
#include "effect/EffectManager.h"
#include "Exceptions.h"


namespace TinyEngine::Util
{
    std::shared_ptr<CubeTexture> DXTextureUtil::RenderIrradianceMap(DirectXGraphics* pGfx, TinyEngine::Graph::DXDefaultRenderGraph&  graph, 
            TinyEngine::Rendering::DirectXCubeTexture& skyboxTexture, int width, int height)
    {
        TextureResDesc desc;
        desc.width = width;
        desc.height = height;
        desc.format = ETextureFormat::RGBA32;
        desc.mipLevels = 1;                                 // need mipmap
        desc.linear = true;
        std::shared_ptr<CubeTexture> irradianceMap = std::make_shared<CubeTexture>(desc);

        auto irradiancePass = EffectManager::Get().FindPass("SkyBox/IrradianceMapPass");
        auto renderTarget = std::make_shared<DirectXRenderTarget>(pGfx, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
        auto depthStencil = std::make_shared<DirectXDepthStencil>(pGfx, width, height);
        
        Matrix4x4 lookat[6];
        Matrix4x4 projection;

        lookat[0] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(1.0f,  0.0f,  0.0f), Vector3::up); // +X
        lookat[1] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(-1.0f,  0.0f,  0.0f), Vector3::up); // -X
        lookat[2] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  1.0f,  0.0f), -Vector3::forward); // +Y
        lookat[3] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  -1.0f,  0.0f), Vector3::forward); // -Y
        lookat[4] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  0.0f,  1.0f), Vector3::up); // +Z
        lookat[5] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  0.0f,  -1.0f), Vector3::up); // -Z

        projection = Matrix4x4::Perspective(90.0f, 1.0f, 0.1f, 10.0f);

        Mesh cubeMesh = Primitive::CreateCubeMesh(3.0f);
        wrl::ComPtr<ID3D11Texture2D> pTexture;
        renderTarget->pTextureView->GetResource(reinterpret_cast<ID3D11Resource**>(pTexture.GetAddressOf()));
        
        wrl::ComPtr<ID3D11Texture2D> pTexForRead;
        D3D11_TEXTURE2D_DESC descForRead = {};
        pTexture->GetDesc(&descForRead);
        descForRead.BindFlags = 0;
        descForRead.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        descForRead.Usage = D3D11_USAGE_STAGING;
        INFOMAN(*pGfx);
        GFX_THROW_INFO(pGfx->GetDevice()->CreateTexture2D(&descForRead, nullptr, &pTexForRead));

        auto & helper = HLSLShaderHelper::Get();
        if(renderTarget && depthStencil) pGfx->BindRenderTarget(renderTarget.get(), depthStencil.get());
        else if(renderTarget) pGfx->BindRenderTarget(renderTarget.get(), nullptr);
        else if(depthStencil) pGfx->BindRenderTarget(nullptr, depthStencil.get());
        
        // set viewport
        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<float>(width);
        viewport.Height = static_cast<float>(height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        pGfx->GetContext()->RSSetViewports(1u, &viewport);

        if(!skyboxTexture.HasCreated()) skyboxTexture.CreateResource(pGfx);
        skyboxTexture.BindAsTexture(pGfx, 0);
        //Render six faces of cubemap
        for(int i = 0 ; i < 6; ++i)
        {
            renderTarget->Clear(pGfx);
            depthStencil->Clear(pGfx);

            helper.SetGlobalMatrix("g_View", lookat[i]);
            helper.SetGlobalMatrix("g_Proj", projection);
            helper.SetGlobalMatrix("g_ViewProj", projection * lookat[i]);

            pGfx->UpdateCBuffer(graph.GetFrameConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerFrameCBufSlot));
            pGfx->ApplyPassToMesh(irradiancePass, &cubeMesh);

            pGfx->GetContext()->CopyResource(pTexForRead.Get(), pTexture.Get());
            D3D11_MAPPED_SUBRESOURCE mapped = {};
            GFX_THROW_INFO(pGfx->GetContext()->Map(pTexForRead.Get(), 0u, D3D11_MAP_READ, 0u, &mapped));
            auto pTexData = reinterpret_cast<unsigned char*>(mapped.pData); // get data from texture
            
            irradianceMap->textures[i]->SetData(pTexData, mapped.RowPitch * height, 0);
        }

        // for(int i = 0 ; i < 6; i++)
        // {
        //     // use stb_image save data
        //     std::string filename = "res/images/irradiance/" + std::to_string(i) + ".png";
        //     irradianceMap->textures[i]->SavePNG(filename, 0);
        // }
        return irradianceMap;
    }
    std::shared_ptr<CubeTexture> DXTextureUtil::GeneratePrefilterMap(DirectXGraphics* pGfx, TinyEngine::Graph::DXDefaultRenderGraph& graph, 
            TinyEngine::Rendering::DirectXCubeTexture& skyboxTexture, int width, int height, int mipLevels)
    {
        TextureResDesc desc;
        desc.width = width;
        desc.height = height;
        desc.format = ETextureFormat::RGBA32;
        desc.mipLevels = mipLevels;                             // need mipmap
        desc.linear = true;                                     
        auto outputTex = std::make_shared<CubeTexture>(desc);
        for(int i = 0; i < 6; i++)
            outputTex->textures[i]->generateMipChain();
        Matrix4x4 lookat[6];
        Matrix4x4 projection;
        lookat[0] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(1.0f,  0.0f,  0.0f), Vector3::up); // +X
        lookat[1] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(-1.0f,  0.0f,  0.0f), Vector3::up); // -X
        lookat[2] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  1.0f,  0.0f), -Vector3::forward); // +Y
        lookat[3] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  -1.0f,  0.0f), Vector3::forward); // -Y
        lookat[4] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  0.0f,  1.0f), Vector3::up); // +Z
        lookat[5] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  0.0f,  -1.0f), Vector3::up); // -Z
        projection = Matrix4x4::Perspective(90.0f, 1.0f, 0.1f, 10.0f);

        auto & helper = HLSLShaderHelper::Get();

        if(!skyboxTexture.HasCreated()) skyboxTexture.CreateResource(pGfx);
        //Render six faces of cubemap
        auto skyBoxEffect = EffectManager::Get().FindEffect("SkyBox");

        int curWidth = width;
        auto pMat = Material::Create(skyBoxEffect, "tmp");
        
        auto prefilterPass = skyBoxEffect->FindPass("PrefilterMapPass");
        pGfx->ApplyPass(prefilterPass);
        Mesh cubeMesh = Primitive::CreateCubeMesh(3.0f);
        TinyEngine::RenderEntity entity;
        entity.m_mesh = &cubeMesh;
        entity.m_material = pMat.get();

        std::vector<int> mipSizes(mipLevels);
        for(int i = 0; i < mipLevels; i++)  // for 128: 128, 64, 32, 16, 8
        {
            mipSizes[i] = curWidth;
            curWidth /= 2;
        }
        
        for(int level = 0 ; level < mipLevels; level++)
        {
            int width = mipSizes[level], height = mipSizes[level];
            float roughness = (float)level / (float)(mipLevels - 1);
            pMat->SetFloat("roughness", roughness);
            for(int i = 0; i < 6; i++)
            {
                auto renderTarget = std::make_shared<DirectXRenderTarget>(pGfx, width, height);
                auto depthStencil = std::make_shared<DirectXDepthStencil>(pGfx, width, height);
                if(renderTarget && depthStencil) pGfx->BindRenderTarget(renderTarget.get(), depthStencil.get());
                else if(renderTarget) pGfx->BindRenderTarget(renderTarget.get(), nullptr);
                else if(depthStencil) pGfx->BindRenderTarget(nullptr, depthStencil.get());

                // set viewport
                //!!!!!!!!!!!!!! I set the viewport in BindRenderTarget()!!!!,  it cause I have to reset viewport here!!!!
                D3D11_VIEWPORT viewport = {};
                viewport.TopLeftX = 0;
                viewport.TopLeftY = 0;
                viewport.Width =  static_cast<float>(width);
                viewport.Height = static_cast<float>(height);
                viewport.MinDepth = 0.0f;
                viewport.MaxDepth = 1.0f;

                pGfx->GetContext()->RSSetViewports(1u, &viewport);

                renderTarget->Clear(pGfx);
                depthStencil->Clear(pGfx);
                
                skyboxTexture.BindAsTexture(pGfx, 0);
                helper.SetGlobalMatrix( "g_View", lookat[i]);
                helper.SetGlobalMatrix("g_Proj", projection);
                helper.SetGlobalMatrix("g_ViewProj", projection * lookat[i]);
                pGfx->UpdateCBuffer(graph.GetFrameConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerFrameCBufSlot));
                pGfx->Draw(&entity);

                wrl::ComPtr<ID3D11Texture2D> pTexture;
                renderTarget->pTextureView->GetResource(reinterpret_cast<ID3D11Resource**>(pTexture.GetAddressOf()));
                wrl::ComPtr<ID3D11Texture2D> pTexForRead;
                D3D11_TEXTURE2D_DESC descForRead = {};
                pTexture->GetDesc(&descForRead);
                descForRead.BindFlags = 0;
                descForRead.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                descForRead.Usage = D3D11_USAGE_STAGING;
                INFOMAN(*pGfx);
                GFX_THROW_INFO(pGfx->GetDevice()->CreateTexture2D(&descForRead, nullptr, &pTexForRead));
                pGfx->GetContext()->CopyResource(pTexForRead.Get(), pTexture.Get());
                D3D11_MAPPED_SUBRESOURCE mapped = {};
                GFX_THROW_INFO(pGfx->GetContext()->Map(pTexForRead.Get(), 0u, D3D11_MAP_READ, 0u, &mapped));
                auto pTexData = reinterpret_cast<unsigned char*>(mapped.pData); // get data from texture
                long numBytes = height * width * 4 * sizeof(unsigned char);
                outputTex->textures[i]->SetData(pTexData, numBytes, level);
            }
        }

        // for(int i = 0 ; i < 6; i++)
        // {
        //     for(int j = 0; j < mipLevels; j++)
        //     {
        //         std::string path = "res/images/prefilter/" + std::to_string(i) + "_mip" + std::to_string(j) + ".png";
        //         outputTex->textures[i]->SavePNG(path, j);
        //     }
        // }
        return outputTex;
    }

    std::shared_ptr<Texture> DXTextureUtil::GenerateBRDFLUT(DirectXGraphics* pGfx, TinyEngine::Graph::DXDefaultRenderGraph & graph,
             bool saveToFile, unsigned int width, unsigned int height)
    {
        std::shared_ptr<Texture> outputTex = std::make_shared<Texture>(width, height, ETextureFormat::RGHalf);
        outputTex->SetWrapMode(EWrapMode::Clamp);
        auto brdfPass = EffectManager::Get().FindPass("BRDFLUT/BRDFLUTPass");
        
        Mesh quadMesh = Primitive::CreateQuadMesh();
        auto renderTarget = std::make_shared<DirectXRenderTarget>(pGfx, width, height, DXGI_FORMAT_R16G16_FLOAT);
        renderTarget->Clear(pGfx);
        pGfx->BindRenderTarget(renderTarget.get(), nullptr);
        pGfx->ApplyPassToMesh(brdfPass, &quadMesh);

        wrl::ComPtr<ID3D11Texture2D> pTexture;
        renderTarget->pTextureView->GetResource(reinterpret_cast<ID3D11Resource**>(pTexture.GetAddressOf()));
        wrl::ComPtr<ID3D11Texture2D> pTexForRead;
        D3D11_TEXTURE2D_DESC descForRead = {};
        pTexture->GetDesc(&descForRead);
        descForRead.BindFlags = 0;
        descForRead.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        descForRead.Usage = D3D11_USAGE_STAGING;
        INFOMAN(*pGfx);
        GFX_THROW_INFO(pGfx->GetDevice()->CreateTexture2D(&descForRead, nullptr, &pTexForRead));
        pGfx->GetContext()->CopyResource(pTexForRead.Get(), pTexture.Get());
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        
        GFX_THROW_INFO(pGfx->GetContext()->Map(pTexForRead.Get(), 0u, D3D11_MAP_READ, 0u, &mapped));
        long numBytes = height * width * 2 * sizeof(uint16_t);
        outputTex->SetData(mapped.pData, numBytes);

        if(saveToFile)
            outputTex->SavePNG("res/images/brdfLUT.png");
        return outputTex;
    }
}