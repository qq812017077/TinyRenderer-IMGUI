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
            RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXDepthStencil>>("shadowmap", shadowMapHandle));
            RegisterSource(std::make_unique<SourceAttachment<TinyEngine::DirectXDepthStencil>>("shadowmap", shadowMapHandle));
            RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DepthCubeTexture>>("shadowcubemap", shadowCubeMapHandle));
            RegisterSource(std::make_unique<SourceAttachment<TinyEngine::DepthCubeTexture>>("shadowcubemap", shadowCubeMapHandle));

            dir_shadowPass = EffectManager::Get().FindPass("Default/ShadowCastPass");
            dir_shadowPass.psName = "none";
            dir_shadowPass.rasterDesc.DepthBias = 40;
            dir_shadowPass.rasterDesc.SlopeScaledDepthBias =  2.5f;
            dir_shadowPass.rasterDesc.DepthBiasClamp = 1.00f;

            point_ShadowPass = ShaderPass::Get("PointShadowPass", "shaders/effects/PointShadowVS.hlsl", "shaders/effects/PointShadowPS.hlsl");
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
            auto * shadowMap = graph.GetBufferResource(shadowMapHandle);
            auto * shadowCubeMap = graph.GetBufferResource(shadowCubeMapHandle);
            pGfx->BindRenderTarget(nullptr, shadowMap);
            
            // for directional light, we need bind light transform, view, proj matrix
            auto scene = pScene.get();
            auto & helper = HLSLShaderHelper::Get();
            
            // for directional light
            helper.SetGlobalVariable("g_DirLight", &scene->m_directional_light.m_buffer, sizeof(TinyEngine::DirectionalLight::DirectionalLightBuffer));
            helper.SetGlobalMatrix("g_LightVP", scene->m_directional_light.m_lightViewProj);
            
            pGfx->UpdateCBuffer(graph.GetLightingConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerLightingCBufSlot));
            // draw all objects that cast shadow
            pGfx->ApplyPassToRenderList(dir_shadowPass, scene->m_directional_light.visibleRenderers); // load shader and render state
            
            //for points light
            auto & light = scene->m_point_lights[0];
            helper.SetGlobalMatrix("g_LightProj", light.m_lightProj);
            helper.SetGlobalVariable("g_PointLight", &light.m_buffer, sizeof(TinyEngine::PointLight::PointLightBuffer));
            for(int face = 0 ; face < 6; face++)
            {
                auto depthMap = shadowCubeMap->GetDepthBuffer(face);
                depthStencil->Clear(pGfx);
                pGfx->BindRenderTarget(depthMap, depthStencil.get());
                pGfx->SetViewport(Graphics::ViewPort{0, 0, depthMap->GetWidth(), depthMap->GetHeight()});
                helper.SetGlobalMatrix("g_LightView", light.m_lightView[face]);
                helper.SetGlobalMatrix("g_LightViewProj", light.m_lightViewProj[face]);
                pGfx->UpdateCBuffer(graph.GetLightingConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerLightingCBufSlot));
                pGfx->ApplyPassToRenderList(point_ShadowPass, scene->m_point_lights[0].visibleRenderers); // load shader and render state
            }
        }

    private:
        ResourceHandle<TinyEngine::DirectXDepthStencil> shadowMapHandle;
        ResourceHandle<TinyEngine::DepthCubeTexture> shadowCubeMapHandle;
        std::shared_ptr<TinyEngine::DirectXDepthStencil> depthStencil{nullptr};
        ShaderPass dir_shadowPass;
        ShaderPass point_ShadowPass;
    };
}