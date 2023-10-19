#include "Graphics.h"
#include "EffectPass.h"
#include "scene/Scene.h"
#include "HLSLShaderHelper.h"
#include "effect/Effect.h"
#include "effect/Pass.h"
#include "graphics/RenderTarget.h"
#include "graphics/DepthStencil.h"

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
        
    }


    MousePickEffectPass::MousePickEffectPass() : EffectPassBase()
    {
        enabled = true;
        
        maskPass = EffectManager::Get().FindPass("Default/DefaultPass");
        maskPass.passName = "MaskPass";
        maskPass.depthStencilDesc.depthMode = TinyEngine::EDepthMode::Off;
        maskPass.depthStencilDesc.stencilMode = TinyEngine::Rendering::EStencilMode::WriteMask;
        maskPass.cullMode = TinyEngine::ECullMode::Off;
        maskPass.psName = "none";

        outlinePass = EffectManager::Get().FindPass("Outline/OutlinePass");
        outlinePass.depthStencilDesc.depthMode = TinyEngine::EDepthMode::Off;
        outlinePass.depthStencilDesc.stencilMode = TinyEngine::Rendering::EStencilMode::ReadMask;
        outlinePass.cullMode = TinyEngine::ECullMode::Off;

        fullscreenPass = EffectManager::Get().FindPass("Blur/BlurPass");
        // outlinePass.depthStencilDesc.depthMode = TinyEngine::EDepthMode::Off;
        fullscreenPass.blendDesc = BlendDesc::BlendOneOne();
    }

    MousePickEffectPass::~MousePickEffectPass()
    {

    }

    void MousePickEffectPass::Apply(Graphics* pGfx, std::shared_ptr<Scene> scene)
    {
        if(scene->selectedRenderers.size() == 0) return;
        if(pRenderTarget == nullptr || pRenderTarget->GetWidth() != pGfx->GetWidth() || pRenderTarget->GetHeight() != pGfx->GetHeight())
        {
            pRenderTarget = pGfx->CreateRenderTarget();
            pDepthStencil = pGfx->CreateDepthStencil();
        }

        pRenderTarget->Clear(pGfx);
        pDepthStencil->Clear(pGfx);
        pGfx->BindRenderTarget(pRenderTarget.get(), pDepthStencil.get());

        // mask
        pGfx->Apply(maskPass, scene->selectedRenderers); // load shader and render state

        // outline
        for(auto & renderer : scene->selectedRenderers)
        {
            if(!renderer) continue;
            auto pTransform = renderer->pTransform;
            auto scale = pTransform->GetScale();
            pTransform->SetScale(scale * 1.05f);
        }
        pGfx->Apply(outlinePass, scene->selectedRenderers);
        
        // restore scale
        for(auto & renderer : scene->selectedRenderers)
        {
            if(!renderer) continue;
            auto pTransform = renderer->pTransform;
            auto scale = pTransform->GetScale();
            pTransform->SetScale(scale / 1.05f);
        }
        
        // output to screen
        pGfx->BindDefaultRenderTarget(); // bind to default render target
        pGfx->ApplyToRenderTarget(fullscreenPass, pRenderTarget.get());
    }
}