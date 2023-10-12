#include "Graphics.h"
#include "EffectPass.h"
#include "scene/Scene.h"
#include "HLSLShaderHelper.h"
#include "effect/Effect.h"

namespace TinyEngine
{
    ShadingEffectPass::ShadingEffectPass()
    : EffectPassBase()
    {
        enabled = true;
    }

    ShadingEffectPass::~ShadingEffectPass()
    {

    }



    void ShadingEffectPass::Apply(Graphics* pGfx, std::shared_ptr<Scene> scene)
    {
        //逐光源渲染
        // camera config
        // auto & helper = HLSLShaderHelper::Get();
        // helper.SetGlobalMatrix("g_View", scene->m_main_camera->GetViewMatrix());
        // helper.SetGlobalMatrix("g_Proj", scene->m_main_camera->GetProjectionMatrix());
        // helper.SetGlobalVector("g_EyePos", scene->m_main_camera->pTransform->GetPosition());

        // // get directional light
        // helper.SetGlobalVariable("g_DirLight", &scene->m_directional_light, sizeof(TinyEngine::DirectionalLight));

        // for(size_t i=0; i < scene->m_point_lights.size(); ++i)
        // {
        //     helper.SetGlobalVariable("g_PointLight", &scene->m_point_lights[i], sizeof(TinyEngine::PointLight));
        // }
        
        // pGfx->UpdateCBuffer(pFrameBuffer, helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerFrameCBufSlot));
        // pGfx->UpdateCBuffer(pLightingBuffer, helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerLightingCBufSlot));
        // for(size_t i=0; i < scene->effectDescs.size(); ++i)
        // {
        //     auto & desc = scene->effectDescs[i];
        //     auto pEffect = desc.pEffect;
        //     auto & renderers = desc.renderers;

        //     for(size_t j=0; j < pEffect->GetPassCount(); ++j)
        //     {
        //         auto pass = pEffect->GetPass(j);
        //         pGfx->Apply(pass, renderers); // load shader and render state
                
        //     }
        // }

        // get all objects;
        // for each mesh: get pass and lights
        // set render config, and render
    }


    MousePickEffectPass::MousePickEffectPass() : EffectPassBase()
    {
        enabled = true;
        
        maskPass = EffectManager::Get().FindPass("Default/DefaultPass");
        maskPass.passName = "MaskPass";
        maskPass.depthStencilDesc.depthMode = TinyEngine::EDepthMode::Off;
        maskPass.depthStencilDesc.stencilMode = TinyEngine::Rendering::EStencilMode::WriteMask;
        maskPass.psName = "none";

        outlinePass = EffectManager::Get().FindPass("Outline/OutlinePass");
        outlinePass.depthStencilDesc.depthMode = TinyEngine::EDepthMode::Off;
        outlinePass.depthStencilDesc.stencilMode = TinyEngine::Rendering::EStencilMode::ReadMask;
    }

    MousePickEffectPass::~MousePickEffectPass()
    {
        
    }

    void MousePickEffectPass::Apply(Graphics* pGfx, std::shared_ptr<Scene> scene)
    {
        if(scene->selectedRenderers.size() == 0) return;

        // mask
        pGfx->Apply(maskPass, scene->selectedRenderers); // load shader and render state

        // outline
        for(auto & renderer : scene->selectedRenderers)
        {
            if(!renderer) continue;
            auto pTransform = renderer->pTransform;
            auto scale = pTransform->GetScale();
            pTransform->SetScale(scale * 1.1f);
        }
        pGfx->Apply(outlinePass, scene->selectedRenderers);
        // restore scale
        for(auto & renderer : scene->selectedRenderers)
        {
            if(!renderer) continue;
            auto pTransform = renderer->pTransform;
            auto scale = pTransform->GetScale();
            pTransform->SetScale(scale / 1.1f);
        }
    }
}