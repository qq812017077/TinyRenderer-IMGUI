#include "BufferPass.h"
#include "graph/RenderGraph.h"
#include "graph/Attachment.h"
#include "graphics/BufferResource.h"
#include "dxgraphics/DirectXRenderTarget.h"
#include "dxgraphics/DirectXDepthStencil.h"

namespace TinyEngine::Graph
{
    BufferPass::BufferPass(std::string name) : RenderPass(name)
    {
        RegisterSink(std::make_unique<SinkAttachment<TinyEngine::BufferResource>>("buffer", bufferhandle));
        RegisterSource(std::make_unique<SourceAttachment<TinyEngine::BufferResource>>("buffer", bufferhandle));
    }
    
    void BufferPass::Initialize(RenderGraph & graph)
    {
        
    }

    
    void BufferPass::Execute(Graphics *pGfx, RenderGraph& graph)
    {
        auto buffer = graph.GetBufferResource(bufferhandle);
        buffer->Clear(pGfx);
    }
}