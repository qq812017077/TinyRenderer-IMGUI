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
#include "global/RuntimeGlobalContext.h"
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
            RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DitectXDepthArray>>("shadowmap", shadowMapArrayHandle));
            RegisterSource(std::make_unique<SourceAttachment<TinyEngine::DitectXDepthArray>>("shadowmap", shadowMapArrayHandle));
            RegisterSink(std::make_unique<SinkAttachment<TinyEngine::CubeRenderTexture>>("shadowcubemap", shadowCubeMapHandle));
            RegisterSource(std::make_unique<SourceAttachment<TinyEngine::CubeRenderTexture>>("shadowcubemap", shadowCubeMapHandle));

            dir_shadowPass = EffectManager::Get().FindPass("Default/ShadowCastPass");
            dir_shadowPass.psName = "none";
            dir_shadowPass.rasterDesc.DepthBias = g_runtime_global_context.m_shadow_config.m_point_shadow_config.depth_bias;
            dir_shadowPass.rasterDesc.SlopeScaledDepthBias = g_runtime_global_context.m_shadow_config.m_point_shadow_config.slope_scaled_bias;
            dir_shadowPass.rasterDesc.DepthBiasClamp = g_runtime_global_context.m_shadow_config.m_point_shadow_config.depth_bias_clamp;

            point_ShadowPass = ShaderPass::Get("PointShadowPass", "shaders/effects/PointShadowVS.hlsl", "shaders/effects/PointShadowPS.hlsl");
            point_ShadowPass.rasterDesc.DepthBias = g_runtime_global_context.m_shadow_config.m_point_shadow_config.depth_bias;
            point_ShadowPass.rasterDesc.SlopeScaledDepthBias = g_runtime_global_context.m_shadow_config.m_point_shadow_config.slope_scaled_bias;
            point_ShadowPass.rasterDesc.DepthBiasClamp = g_runtime_global_context.m_shadow_config.m_point_shadow_config.depth_bias_clamp;
        }
        virtual ~ShadowPass() = default;

        void Initialize(Graphics* pGfx, RenderGraph & graph) override
        {
            unsigned int shadowCubeSize = 1024;
            depthStencil = std::make_shared<DirectXDepthStencil>(dynamic_cast<DirectXGraphics*>(pGfx), 
                    shadowCubeSize, shadowCubeSize);
        }

        void internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph) override
        {
            auto * shadowMapArray = graph.GetBufferResource(shadowMapArrayHandle);
            auto * shadowCubeMap = graph.GetBufferResource(shadowCubeMapHandle);
            // for directional light, we need bind light transform, view, proj matrix
            auto scene = pScene.get();
            auto & helper = HLSLShaderHelper::Get();
            
            // for directional light
            if(scene->m_directional_light.exist)
            {
                helper.SetGlobalVariable("g_DirLight", &scene->m_directional_light.m_buffer, sizeof(TinyEngine::DirectionalLight::DirectionalLightBuffer));
                if(scene->m_directional_light.use_csm)
                {
                    int num_splits = scene->m_directional_light.m_csm.m_split_num;
                    if(num_splits != 4) THROW_ENGINE_EXCEPTION("only support 4 splits now");

                    for(int i = 0; i < num_splits; i++)
                    {
                        auto targetBuffer = shadowMapArray->GetIndexBuffer(i);
                        pGfx->BindRenderTarget(nullptr, targetBuffer);
                        helper.SetGlobalMatrix("g_LightViewProj", scene->m_directional_light.m_csm.m_split_view_proj[i]);
                        pGfx->UpdateCBuffer(graph.GetLightingConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerLightingCBufSlot));
                        // draw all objects that cast shadow
                        pGfx->ApplyPassToRenderList(dir_shadowPass, scene->m_directional_light.m_csm.m_split_visible_renderers[i]); // load shader and render state
                    }
                }else{
                    auto targetBuffer = shadowMapArray->GetIndexBuffer(0);
                    pGfx->BindRenderTarget(nullptr, targetBuffer);
                    helper.SetGlobalMatrix("g_LightViewProj", scene->m_directional_light.m_nsm.m_view_proj);
                    pGfx->UpdateCBuffer(graph.GetLightingConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerLightingCBufSlot));
                    // draw all objects that cast shadow
                    pGfx->ApplyPassToRenderList(dir_shadowPass, scene->m_directional_light.m_nsm.visibleRenderers); // load shader and render state
                }
            }
            
            
            //for points light
            if(scene->m_point_lights.size() > 0)
            {
                auto & light = scene->m_point_lights[0];
                helper.SetGlobalMatrix("g_LightProj", light.m_lightProj);
                helper.SetGlobalVariable("g_PointLight", &light.m_buffer, sizeof(TinyEngine::PointLight::PointLightBuffer));
                for(int face = 0 ; face < 6; face++)
                {
                    auto depthMap = shadowCubeMap->GetFaceBuffer(face);
                    depthStencil->Clear(pGfx);
                    pGfx->BindRenderTarget(depthMap, depthStencil.get());
                    pGfx->SetViewport(Graphics::ViewPort{0, 0, depthMap->GetWidth(), depthMap->GetHeight()});
                    helper.SetGlobalMatrix("g_LightView", light.m_lightView[face]);
                    helper.SetGlobalMatrix("g_LightViewProj", light.m_lightViewProj[face]);
                    pGfx->UpdateCBuffer(graph.GetLightingConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerLightingCBufSlot));
                    pGfx->ApplyPassToRenderList(point_ShadowPass, scene->m_point_lights[0].visibleRenderers); // load shader and render state
                }
            }
        }

    private:
        ResourceHandle<TinyEngine::DitectXDepthArray> shadowMapArrayHandle;
        ResourceHandle<TinyEngine::CubeRenderTexture> shadowCubeMapHandle;
        std::shared_ptr<TinyEngine::DirectXDepthStencil> depthStencil{nullptr};
        ShaderPass dir_shadowPass;
        ShaderPass point_ShadowPass;
    };
}