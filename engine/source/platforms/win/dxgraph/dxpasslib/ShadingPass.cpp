#include "ShadingPass.h"
#include "graph/RenderResource.h"
#include "graph/RenderGraph.h"
#include "graphics/Graphics.h"
#include "dxgraphics/DirectXRenderTarget.h"
#include "dxgraphics/DirectXDepthStencil.h"
#include "scene/Scene.h"
#include "effect/Effect.h"
#include "directX/HLSLShaderHelper.h"
#include "dxgraph/DirectXRenderGraph.h"
namespace TinyEngine::Graph
{
    ShadingPass::ShadingPass(std::string name) : RasterPass(name)
    {
        RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXDepthStencil>>("shadowmap", shadowMapHandle));
        RegisterSink(std::make_unique<SinkAttachment<TinyEngine::CubeRenderTexture>>("shadowcubemap", shadowCubeMapHandle));
    }
    
    void ShadingPass::BindScene(std::shared_ptr<TinyEngine::Scene> pScene) noexcept
    {
        this->pScene = pScene;
        // bind irradiance map and specular map for pbr
        irradianceMap.BindCubeMap(this->pScene->p_map_resource->m_irradiance_cubemap);
        prefilterMap.BindCubeMap(this->pScene->p_map_resource->m_prefilter_cubemap);
        pBrdfLutMap.BindTexture(this->pScene->p_map_resource->m_brdf_lut_map);
    }

    void ShadingPass::internalExecute(DirectXGraphics *pGfx, DXDefaultRenderGraph& graph)
    {
        auto * shadowMap = graph.GetBufferResource(shadowMapHandle);
        auto * shadowCubeMap = graph.GetBufferResource(shadowCubeMapHandle);
        
        // bind irradiance map and specular map for pbr
        if(!irradianceMap.HasCreated()) irradianceMap.CreateResource(pGfx);
        irradianceMap.BindAsTexture(pGfx, HLSLShaderHelper::IrradianceMapSlot);
        
        if(!prefilterMap.HasCreated()) prefilterMap.CreateResource(pGfx);
        prefilterMap.BindAsTexture(pGfx, HLSLShaderHelper::PrefilterMapSlot);
        
        if(!pBrdfLutMap.HasCreated()) pBrdfLutMap.CreateResource(pGfx);
        pBrdfLutMap.BindAsTexture(pGfx, HLSLShaderHelper::BrdfLutMapSlot);
        // need bind scene
        auto scene = pScene.get();
        auto & helper = HLSLShaderHelper::Get();
        helper.SetGlobalMatrix("g_View", scene->m_main_camera->GetViewMatrix());
        helper.SetGlobalMatrix("g_Proj", scene->m_main_camera->GetProjectionMatrix());

        helper.SetGlobalMatrix("g_ViewProj", scene->m_main_camera->GetProjectionMatrix() * scene->m_main_camera->GetViewMatrix());
        helper.SetGlobalVector("g_EyePos", scene->m_main_camera->pTransform->GetPosition());

        // for directional light
        helper.SetGlobalVariable("g_DirLight", &scene->m_directional_light.m_buffer, sizeof(TinyEngine::DirectionalLight::DirectionalLightBuffer));
        helper.SetGlobalMatrix("g_LightViewProj", scene->m_directional_light.m_lightViewProj);
        shadowMap->BindAsTexture(pGfx, HLSLShaderHelper::ShadowMapSlot);

        // for point light
        helper.SetGlobalVariable("g_PointLight", &scene->m_point_lights[0].m_buffer, sizeof(TinyEngine::PointLight::PointLightBuffer));

        shadowCubeMap->BindAsTexture(pGfx, HLSLShaderHelper::ShadowCubeMapSlot);
        pGfx->BindSampler(0, shadowMap->GetSamplerState(), Graphics::EBindType::ToPS);

        pGfx->UpdateCBuffer(graph.GetFrameConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerFrameCBufSlot));
        pGfx->UpdateCBuffer(graph.GetLightingConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerLightingCBufSlot));
        for(size_t i=0; i < scene->CamVisibleRenderers.size(); ++i)
        {
            auto & desc = scene->CamVisibleRenderers[i];
            auto pEffect = desc.pEffect;
            auto & renderers = desc.renderers;
            if(renderers.empty()) continue;
            for(size_t j=0; j < pEffect->GetPassCount(); ++j)
            {
                auto pass = pEffect->GetPass(j);
                if(pass.lightMode == ELightMode::ShadowCaster) continue;
                pGfx->ApplyPass(pass); // load shader and render state
                for(auto & entity : desc.renderEntities)
                    pGfx->Draw(&entity);
            }
        }
    }
}
    