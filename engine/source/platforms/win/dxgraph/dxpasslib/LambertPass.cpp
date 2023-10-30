#include "LambertPass.h"
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
    LambertPass::LambertPass(std::string name) : RasterPass(name)
    {
        RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXDepthStencil>>("shadowmap", shadowMapHandle));
        RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DepthCubeTexture>>("shadowcubemap", shadowCubeMapHandle));
    }
    

    void LambertPass::internalExecute(DirectXGraphics *pGfx, DXDefaultRenderGraph& graph)
    {
        auto * shadowMap = graph.GetBufferResource(shadowMapHandle);
        auto * shadowCubeMap = graph.GetBufferResource(shadowCubeMapHandle);
        // need bind scene
        auto scene = pScene.get();
        auto & helper = HLSLShaderHelper::Get();
        // helper.SetGlobalMatrix("g_ViewProj", scene->m_point_lights[0].m_lightViewProj[3]);
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
                pGfx->ApplyPassToRenderList(pass, renderers); // load shader and render state
            }
        }
    }
}
    