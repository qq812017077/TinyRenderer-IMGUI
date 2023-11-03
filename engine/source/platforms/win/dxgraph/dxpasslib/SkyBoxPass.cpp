#include "SkyBoxPass.h"
#include "stb/stb_image_write.h"
#include "Texture.h"

namespace TinyEngine::Graph
{

    void SkyBoxPass::RenderCubeMap(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph, Renderer * pRenderer, int width, int height)
    {
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
        //Render six faces of cubemap
        for(int i = 0 ; i < 6; ++i)
        {
            renderTarget->Clear(pGfx);
            depthStencil->Clear(pGfx);

            helper.SetGlobalMatrix("g_View", lookat[i]);
            helper.SetGlobalMatrix("g_Proj", projection);
            helper.SetGlobalMatrix("g_ViewProj", projection * lookat[i]);

            pGfx->UpdateCBuffer(graph.GetFrameConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerFrameCBufSlot));
            auto pEffect = pRenderer->GetMaterialPtr()->GetEffect();
            auto entity = pRenderer->GetRenderEntity();
            for(size_t j=0; j < pEffect->GetPassCount(); ++j)
            {
                auto pass = pEffect->GetPass(j);
                if(pass.lightMode == ELightMode::ShadowCaster) continue;
                pGfx->ApplyPass(pass); // load shader and render state
                pGfx->Draw(&entity);
            }

            pGfx->GetContext()->CopyResource(pTexForRead.Get(), pTexture.Get());
            D3D11_MAPPED_SUBRESOURCE mapped = {};
            GFX_THROW_INFO(pGfx->GetContext()->Map(pTexForRead.Get(), 0u, D3D11_MAP_READ, 0u, &mapped));
            auto pTexData = reinterpret_cast<unsigned char*>(mapped.pData); // get data from texture
            
            // use stb_image save data
            std::string filename = "res/images/skybox-test/" + std::to_string(i) + ".png";
            stbi_write_png(filename.c_str(), width, height, 4, pTexData, mapped.RowPitch);
            pGfx->GetContext()->Unmap(pTexForRead.Get(), 0u);
        }
        pGfx->GetContext()->CopyResource(pTexForRead.Get(), pTexture.Get());
        //restore viewport
        pGfx->GetContext()->RSSetViewports(1u, &pGfx->GetEditorViewPort());
        return ;
    }

    
    void SkyBoxPass::RenderIrradianceMap(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph, int width, int height)
    {
        std::shared_ptr<CubeTexture> irradianceMap;

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

        for(int i = 0 ; i < 6; i++)
        {
            // use stb_image save data
            std::string filename = "res/images/irradiance/" + std::to_string(i) + ".png";
            irradianceMap->textures[i]->SavePNG(filename, 0);
        }
        
    }

    /**
     * @brief 
     *  
     * NOTE:
     *      I found that when When I set width and height is 128. 
     *      mipmmap-5 is 8*8, and data size should be 8 * 8 * 4 * sizeof(unsigned char) = 256. But it gives me 512. 
     *      I don't know why! 
     *      
     *      So I set width and height to 256, and it works( mipmap-5 is 16*16).
     * 
     * @param pGfx 
     * @param graph 
     * @param width 
     * @param height 
     * @param mipLevels 
     * @return std::shared_ptr<CubeTexture> 
     */
    std::shared_ptr<CubeTexture> SkyBoxPass::GeneratePrefilterMap(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph, int width, int height, int mipLevels)
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

        for(int i = 0 ; i < 6; i++)
        {
            for(int j = 0; j < mipLevels; j++)
            {
                std::string path = "res/images/prefilter/" + std::to_string(i) + "_mip" + std::to_string(j) + ".png";
                outputTex->textures[i]->SavePNG(path, j);
            }
        }
        return outputTex;
    }
}