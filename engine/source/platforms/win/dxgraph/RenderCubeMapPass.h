#pragma once
#include "dxgraph/dxpasslib/RasterPass.h"
#include <d3d11.h>
#include <wrl.h>
#include "dxgraphics/DirectXRenderTarget.h"
#include "dxgraphics/DirectXDepthStencil.h"
#include "effect/Pass.h"
#include "effect/Effect.h"
#include "scene/Scene.h"
#include "geometry/Primitive.h"
#include "HLSLShaderHelper.h"
#include "DirectXGraphics.h"
#include "dxgraph/DirectXRenderGraph.h"
#include "RenderEntity.h"
#include "Exceptions.h"
#include "core/math/Matrix.h"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb/stb_image_write.h"

namespace wrl = Microsoft::WRL;
class DirectXGraphics;


namespace TinyEngine
{
    class RenderCubeMapPass
    {
    public:
        RenderCubeMapPass(DirectXGraphics* pGfx, Microsoft::WRL::ComPtr<ID3D11Buffer>& frameBuffer) : pFrameBuffer(frameBuffer)
        {
            renderTarget = std::make_shared<DirectXRenderTarget>(pGfx, 512, 512, DXGI_FORMAT_R8G8B8A8_UNORM);
            depthStencil = std::make_shared<DirectXDepthStencil>(pGfx, 512, 512);
            
            lookat[0] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(1.0f,  0.0f,  0.0f), Vector3::up); // +X
            lookat[1] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(-1.0f,  0.0f,  0.0f), Vector3::up); // -X
            lookat[2] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  1.0f,  0.0f), -Vector3::forward); // +Y
            lookat[3] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  -1.0f,  0.0f), Vector3::forward); // -Y
            lookat[4] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  0.0f,  1.0f), Vector3::up); // +Z
            lookat[5] = Matrix4x4::LookAtLH(Vector3::zero, Vector3(0.0f,  0.0f,  -1.0f), Vector3::up); // -Z

            projection = Matrix4x4::Perspective(90.0f, 1.0f, 0.1f, 10.0f);
        }

        virtual ~RenderCubeMapPass() = default;

        void RenderCubeMap(DirectXGraphics* pGfx, Renderer * pRenderer)
        {
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
            viewport.Width = 512;
            viewport.Height = 512;
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

                pGfx->UpdateCBuffer(pFrameBuffer, helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerFrameCBufSlot));
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
                // stbi_write_png(filename.c_str(), 512, 512, 4, pTexData, mapped.RowPitch);
                pGfx->GetContext()->Unmap(pTexForRead.Get(), 0u);
            }
            pGfx->GetContext()->CopyResource(pTexForRead.Get(), pTexture.Get());
            
            //restore viewport
            pGfx->GetContext()->RSSetViewports(1u, &pGfx->GetEditorViewPort());
            return ;
        }
    private:
        ShaderPass renderPass;
        std::shared_ptr<DirectXRenderTarget> renderTarget;
        std::shared_ptr<DirectXDepthStencil> depthStencil;
        Microsoft::WRL::ComPtr<ID3D11Buffer>& pFrameBuffer;
        Matrix4x4 lookat[6];
        Matrix4x4 projection;
    };
}