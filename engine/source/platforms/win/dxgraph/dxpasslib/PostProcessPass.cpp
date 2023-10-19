#include "PostProcessPass.h"
#include "effect/Effect.h"
#include "effect/Pass.h"
#include "scene/Scene.h"
#include "DirectXGraphics.h"
#include "dxgraph/DirectXRenderGraph.h"

namespace TinyEngine::Graph
{
    PostProcessPass::PostProcessPass(std::string name) : RasterPass(name)
    {
        RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXRenderTarget>>("postRT", postHandle));

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

    void PostProcessPass::Execute(Graphics *pGfx, RenderGraph& graph)
    {
        internalExecute(reinterpret_cast<DirectXGraphics*>(pGfx), static_cast<DXDefaultRenderGraph&>(graph));
    }

    void PostProcessPass::internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph)
    {
        auto * renderTarget = graph.GetBufferResource(renderTargetHandle);
        auto * depthStencil = graph.GetBufferResource(depthStencilHandle);
        auto * postBuffer = graph.GetBufferResource(postHandle);

        pGfx->BindRenderTarget(postBuffer, depthStencil);
        auto scene = pScene;
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
        
        pGfx->BindRenderTarget(renderTarget, depthStencil);
        pGfx->ApplyToRenderTarget(fullscreenPass, postBuffer);
    }
}