#pragma once
#include "graph/RenderPass.h"
#include <d3d11.h>
#include <wrl.h>
#include "effect/Pass.h"
#include "Graphics.h"
#include "dxgraphics/DirectXRenderTarget.h"
#include "dxgraphics/DirectXDepthStencil.h"
#include "DirectXGraphics.h"
#include "effect/Pass.h"

namespace wrl = Microsoft::WRL;
class DirectXGraphics;

namespace TinyEngine::Graph
{
    class PresentPass : public RenderPass
    {
    public:
        PresentPass(std::string name) : RenderPass(name)
        {
            RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXRenderTarget>>("renderTarget", renderTargetHandle));

            RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXRenderTarget>>("backbuffer", backbufferHandle));
            RegisterSource(std::make_unique<SourceAttachment<TinyEngine::DirectXRenderTarget>>("backbuffer", backbufferHandle));

            fullscreenPass = EffectManager::Get().FindPass("Blit/BlitPass");
        }
        virtual ~PresentPass() = default;

        void Execute(Graphics *pGfx, RenderGraph& graph) override
        {
            auto pdxGfx = reinterpret_cast<DirectXGraphics*>(pGfx);
            auto * renderTarget = graph.GetBufferResource(renderTargetHandle);
            auto * backbuffer = graph.GetBufferResource(backbufferHandle);
            
            pdxGfx->BindRenderTarget(backbuffer, nullptr);
            pdxGfx->GetContext()->RSSetViewports(1, &pdxGfx->GetEditorViewPort());
            pdxGfx->ApplyToRenderTarget(fullscreenPass, renderTarget);
        }

    protected:
        
    private:

        ResourceHandle<TinyEngine::DirectXRenderTarget> renderTargetHandle;
        ResourceHandle<TinyEngine::DirectXRenderTarget> backbufferHandle;
        ShaderPass fullscreenPass;
    };
}