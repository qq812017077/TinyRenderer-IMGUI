#include "DirectXGraphics.h"
#include "DirectXShadingEffectPass.h"
#include "scene/Scene.h"
#include "HLSLShaderHelper.h"
#include "effect/Effect.h"

namespace TinyEngine
{
    DirectXShadingEffectPass::DirectXShadingEffectPass()
    : EffectPassBase()
    {
        enabled = true;
    }

    DirectXShadingEffectPass::~DirectXShadingEffectPass()
    {
    }

    void DirectXShadingEffectPass::internalApply(DirectXGraphics* pGfx, std::shared_ptr<Scene> scene)
    {
        //逐光源渲染
        // camera config
        auto & helper = HLSLShaderHelper::Get();
        helper.SetGlobalMatrix("g_View", scene->m_main_camera->GetViewMatrix());
        helper.SetGlobalMatrix("g_Proj", scene->m_main_camera->GetProjectionMatrix());
        helper.SetGlobalVector("g_EyePos", scene->m_main_camera->pTransform->GetPosition());

        // get directional light
        helper.SetGlobalVariable("g_DirLight", &scene->m_directional_light, sizeof(TinyEngine::DirectionalLight));

        for(size_t i=0; i < scene->m_point_lights.size(); ++i)
        {
            helper.SetGlobalVariable("g_PointLight", &scene->m_point_lights[i], sizeof(TinyEngine::PointLight));
        }
        
        pGfx->UpdateCBuffer(pFrameConstantBuffer, helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerFrameCBufSlot));
        pGfx->UpdateCBuffer(pLightingConstantBuffer, helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerLightingCBufSlot));
        for(size_t i=0; i < scene->effectDescs.size(); ++i)
        {
            auto & desc = scene->effectDescs[i];
            auto pEffect = desc.pEffect;
            auto & renderers = desc.renderers;

            for(size_t j=0; j < pEffect->GetPassCount(); ++j)
            {
                auto pass = pEffect->GetPass(j);
                pGfx->ApplyPassToRenderList(pass, renderers); // load shader and render state
            }
        }

        // get all objects;
        // for each mesh: get pass and lights
        // set render config, and render
    }
}