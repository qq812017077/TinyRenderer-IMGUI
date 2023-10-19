#pragma once
#include <d3d11.h>
#include <wrl.h>

#include "RasterPass.h"
#include "effect/Pass.h"
#include "dxgraphics/DirectXRenderTarget.h"
#include "dxgraphics/DirectXDepthStencil.h"
#include "DirectXGraphics.h"
#include "effect/Pass.h"
#include "scene/Scene.h"
#include "HLSLShaderHelper.h"
#include "dxgraph/DirectXRenderGraph.h"

namespace wrl = Microsoft::WRL;
class DirectXGraphics;

namespace TinyEngine::Graph
{
    class ShadowPass : public DXRenderPass
    {
    public:
        ShadowPass(std::string name) : DXRenderPass(name)
        {
            // RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXRenderTarget>>("renderTarget", renderTargetHandle));

            RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXRenderTarget>>("shadowmap", shadowMapHandle));
            RegisterSource(std::make_unique<SourceAttachment<TinyEngine::DirectXRenderTarget>>("shadowmap", shadowMapHandle));

            shadowPass = EffectManager::Get().FindPass("Default/ShadowCastPass");
            shadowPass.psName = "none";
            shadowPass.rasterDesc.DepthBias = 40;
            shadowPass.rasterDesc.SlopeScaledDepthBias =  2.5f;
            shadowPass.rasterDesc.DepthBiasClamp = 1.00f;
            
        }
        virtual ~ShadowPass() = default;

        void internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph) override
        {
            auto * shadowMap = graph.GetBufferResource(shadowMapHandle);
            pGfx->BindRenderTarget(nullptr, shadowMap);
            
            // for directional light, we need bind light transform, view, proj matrix
            auto scene = pScene.get();
            auto & helper = HLSLShaderHelper::Get();
            // get directional light
            helper.SetGlobalVariable("g_DirLight", &scene->m_directional_light.m_buffer, sizeof(TinyEngine::DirectionalLight::DirectionalLightBuffer));
            helper.SetGlobalMatrix("g_LightVP", scene->m_directional_light.m_lightVP);
            
            pGfx->UpdateCBuffer(graph.GetLightingConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerLightingCBufSlot));
            // draw all objects that cast shadow
            for(size_t i=0; i < scene->ShadowCastDescs.size(); ++i)
            {
                auto & desc = scene->ShadowCastDescs[i];
                auto & renderers = desc.renderers;
                pGfx->ApplyPassToRenderList(shadowPass, renderers); // load shader and render state
            }

            //for points light
            // for(size_t i=0; i < scene->m_point_lights.size(); ++i)
            // {
            //     helper.SetGlobalVariable("g_PointLight", &scene->m_point_lights[i], sizeof(TinyEngine::PointLight));
            //     helper.SetGlobalMatrix("g_LightVP", scene->m_point_lights[i].m_lightVP);
            // }
            
            
        }

    private:
        ResourceHandle<TinyEngine::DirectXDepthStencil> shadowMapHandle;
        ShaderPass shadowPass;
    };
}