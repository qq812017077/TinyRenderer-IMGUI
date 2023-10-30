#include "PostProcessPass.h"
#include "effect/Effect.h"
#include "effect/Pass.h"
#include "scene/Scene.h"
#include "DirectXGraphics.h"
#include "dxgraph/DirectXRenderGraph.h"
#include "geometry/Primitive.h"
#include "Mesh.h"

namespace TinyEngine::Graph
{
    /***
     * PostProcess must close depth write, or we will cannot render pass after it.
    */
    PostProcessPass::PostProcessPass(std::string name) : RasterPass(name)
    {
        RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXRenderTarget>>("postRT", postHandle));

        maskPass = EffectManager::Get().FindPass("Default/DefaultPass");
        maskPass.passName = "MaskPass";
        maskPass.depthStencilDesc.depthMode = TinyEngine::EDepthMode::Off;
        maskPass.depthStencilDesc.stencilMode = TinyEngine::Rendering::EStencilMode::WriteMask;
        maskPass.rasterDesc.cullMode = TinyEngine::ECullMode::Off;
        maskPass.psName = "none";

        outlinePass = EffectManager::Get().FindPass("Outline/OutlinePass");
        outlinePass.depthStencilDesc.depthMode = TinyEngine::EDepthMode::Off;
        outlinePass.depthStencilDesc.stencilMode = TinyEngine::Rendering::EStencilMode::ReadMask;
        outlinePass.rasterDesc.cullMode = TinyEngine::ECullMode::Off;

        fullscreenPass = EffectManager::Get().FindPass("Blur/BlurPass");
        fullscreenPass.depthStencilDesc.depthMode = TinyEngine::EDepthMode::Off;
        fullscreenPass.blendDesc = BlendDesc::BlendOneOne();

        quad = Primitive::CreateQuadMesh();
    }

    void PostProcessPass::Execute(Graphics *pGfx, RenderGraph& graph)
    {
        internalExecute(reinterpret_cast<DirectXGraphics*>(pGfx), static_cast<DXDefaultRenderGraph&>(graph));
    }

    void PostProcessPass::internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph)
    {
        auto * renderTarget = graph.GetBufferResource(renderTargetHandle);
        auto * depthStencil = graph.GetBufferResource(depthStencilHandle);
        auto * PostTexture = graph.GetBufferResource(postHandle);

        pGfx->BindRenderTarget(PostTexture, depthStencil);
        auto scene = pScene;
        // mask
        pGfx->ApplyPassToRenderList(maskPass, scene->selectedRenderers); // load shader and render state

        // outline
        for(auto & renderer : scene->selectedRenderers)
        {
            if(!renderer) continue;
            auto pTransform = renderer->pTransform;
            auto scale = pTransform->GetScale();
            pTransform->SetScale(scale * 1.05f);
        }
        pGfx->ApplyPassToRenderList(outlinePass, scene->selectedRenderers);
        
        // restore scale
        for(auto & renderer : scene->selectedRenderers)
        {
            if(!renderer) continue;
            auto pTransform = renderer->pTransform;
            auto scale = pTransform->GetScale();
            pTransform->SetScale(scale / 1.05f);
        }
        
        pGfx->BindRenderTarget(renderTarget, depthStencil);
        PostTexture->BindAsTexture(pGfx, 0);
        pGfx->ApplyPassToMesh(fullscreenPass, &quad);
        // pGfx->ApplyPassToRenderTarget(fullscreenPass, PostTexture);
    }
}