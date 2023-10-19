#include "RasterPass.h"
#include <memory>
#include "graph/Attachment.h"
#include "graph/RenderResource.h"
#include "graph/RenderGraph.h"
#include "graphics/Graphics.h"
#include "dxgraphics/DirectXRenderTarget.h"
#include "dxgraphics/DirectXDepthStencil.h"
#include "dxgraph/DirectXRenderGraph.h"
namespace TinyEngine::Graph
{

    void DXRenderPass::Execute(Graphics *pGfx, RenderGraph& graph)
    {
        internalExecute(reinterpret_cast<DirectXGraphics*>(pGfx), static_cast<DXDefaultRenderGraph&>(graph));
    }
    RasterPass::RasterPass(std::string name) : DXRenderPass(name)
    {
        RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXRenderTarget>>("renderTarget", renderTargetHandle));
        RegisterSource(std::make_unique<SourceAttachment<TinyEngine::DirectXRenderTarget>>("renderTarget", renderTargetHandle));

        RegisterSink(std::make_unique<SinkAttachment<TinyEngine::DirectXDepthStencil>>("depthStencil", depthStencilHandle));
        RegisterSource(std::make_unique<SourceAttachment<TinyEngine::DirectXDepthStencil>>("depthStencil", depthStencilHandle));
    }
    
    void RasterPass::Initialize(RenderGraph & graph)
    {
        
    }

    void RasterPass::Execute(Graphics *pGfx, RenderGraph& graph)
    {
        // firstly, bind render target and depth stencil
        auto * renderTarget = graph.GetBufferResource(renderTargetHandle);
        auto * depthStencil = graph.GetBufferResource(depthStencilHandle);
        if(renderTarget && depthStencil) pGfx->BindRenderTarget(renderTarget, depthStencil);
        else if(renderTarget) pGfx->BindRenderTarget(renderTarget, nullptr);
        else if(depthStencil) pGfx->BindRenderTarget(nullptr, depthStencil);

        internalExecute(reinterpret_cast<DirectXGraphics*>(pGfx), static_cast<DXDefaultRenderGraph&>(graph));
    }
}
    
    