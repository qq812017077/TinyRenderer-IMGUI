#pragma once
#include "dxgraph/dxpasslib/RasterPass.h"
#include <d3d11.h>
#include <wrl.h>
#include "effect/Pass.h"
#include "effect/Effect.h"
#include "scene/Scene.h"
#include "geometry/Primitive.h"
#include "HLSLShaderHelper.h"
#include "DirectXGraphics.h"
#include "dxgraph/DirectXRenderGraph.h"
#include "RenderEntity.h"
#include "Exceptions.h"

namespace wrl = Microsoft::WRL;
class DirectXGraphics;


namespace TinyEngine::Graph
{
    class PickPass : public DXRenderPass
    {
    public:
        PickPass(std::string name) : DXRenderPass(name){
            RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXRenderTarget>>("renderTarget", goIdTextureHandle));
            RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXDepthStencil>>("depthStencil", depthStencilHandle));
        
            pickPass = ShaderPass::Get("PickPass", "shaders/SimpleVertexShader.hlsl", "shaders/PickPS.hlsl");
            pickPass.rasterDesc.cullMode = TinyEngine::ECullMode::Off;
        }

        virtual ~PickPass() = default;
        void Execute(Graphics *pGfx, RenderGraph& graph) override
        {
            auto * renderTarget = graph.GetBufferResource(goIdTextureHandle);
            auto * depthStencil = graph.GetBufferResource(depthStencilHandle);
            depthStencil->Clear(pGfx);
            if(renderTarget && depthStencil) pGfx->BindRenderTarget(renderTarget, depthStencil);
            else if(renderTarget) pGfx->BindRenderTarget(renderTarget, nullptr);
            else if(depthStencil) pGfx->BindRenderTarget(nullptr, depthStencil);

        internalExecute(reinterpret_cast<DirectXGraphics*>(pGfx), static_cast<DXDefaultRenderGraph&>(graph));
        }
        void internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph) override
        {
            // draw camera frustum
            auto &helper = HLSLShaderHelper::Get();
            pGfx->UpdateCBuffer(graph.GetFrameConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerFrameCBufSlot));
            pGfx->UpdateCBuffer(pGfx->GetObjectConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerDrawCBufSlot));
            
            pGfx->ApplyPass(pickPass);

            for(size_t i=0; i < pScene->CamVisibleRenderers.size(); ++i)
            {
                auto & desc = pScene->CamVisibleRenderers[i];
                auto pEffect = desc.pEffect;
                auto & entities = desc.renderEntities;
                if(entities.empty()) continue;
                
                for(auto & entity : entities) 
                {
                    helper.SetGlobalUINT("g_goid", entity.m_instance_id);
                    pGfx->Draw(&entity);
                }
            }
        }

        size_t GetGameObjectID(DirectXGraphics* pGfx,  RenderGraph& graph, float u, float v)
        {
            // from texture get id
            auto * renderTarget = graph.GetBufferResource(goIdTextureHandle);

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
            auto pTexData = reinterpret_cast<UINT*>(mapped.pData);
            auto id = pTexData[(int)(v * (float)descForRead.Height) * descForRead.Width + (int)(u * (float)descForRead.Width)];
            pGfx->GetContext()->Unmap(pTexForRead.Get(), 0u);
            return id;
        }
    private:
        ShaderPass pickPass;
        ResourceHandle<TinyEngine::DirectXRenderTarget> goIdTextureHandle;
        ResourceHandle<TinyEngine::DirectXDepthStencil> depthStencilHandle;
    };
}