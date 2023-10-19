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
    }
    

    void LambertPass::internalExecute(DirectXGraphics *pGfx, DXDefaultRenderGraph& graph)
    {
        auto * shadowMap = graph.GetBufferResource(shadowMapHandle);
        // need bind scene
        auto scene = pScene.get();
        auto & helper = HLSLShaderHelper::Get();
        helper.SetGlobalMatrix("g_View", scene->m_main_camera->GetViewMatrix());
        helper.SetGlobalMatrix("g_Proj", scene->m_main_camera->GetProjectionMatrix());
        helper.SetGlobalVector("g_EyePos", scene->m_main_camera->pTransform->GetPosition());

        // get directional light
        helper.SetGlobalVariable("g_DirLight", &scene->m_directional_light.m_buffer, sizeof(TinyEngine::DirectionalLight::DirectionalLightBuffer));
        helper.SetGlobalMatrix("g_LightVP", scene->m_directional_light.m_lightVP);

        for(size_t i=0; i < scene->m_point_lights.size(); ++i)
        {
            helper.SetGlobalVariable("g_PointLight", &scene->m_point_lights[i], sizeof(TinyEngine::PointLight));
        }
        shadowMap->BindAsTexture(pGfx, HLSLShaderHelper::ShadowMapSlot);
        pGfx->UpdateCBuffer(graph.GetFrameConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerFrameCBufSlot));
        pGfx->UpdateCBuffer(graph.GetLightingConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerLightingCBufSlot));
        for(size_t i=0; i < scene->effectDescs.size(); ++i)
        {
            auto & desc = scene->effectDescs[i];
            auto pEffect = desc.pEffect;
            auto & renderers = desc.renderers;

            for(size_t j=0; j < pEffect->GetPassCount(); ++j)
            {
                auto pass = pEffect->GetPass(j);
                if(pass.lightMode == ELightMode::ShadowCaster) continue;
                pGfx->Apply(pass, renderers); // load shader and render state
            }
        }
    }
}
    