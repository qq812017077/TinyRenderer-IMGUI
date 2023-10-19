#pragma once
#include "graph/RenderPass.h"
#include <memory>
#include "graph/RenderResource.h"
namespace TinyEngine
{
    class BufferResource;
}
namespace TinyEngine::Graph
{
    class BufferPass : public RenderPass
    {
    public:
        BufferPass(std::string name);
        virtual ~BufferPass() = default;
        void Initialize(RenderGraph & graph) override;
        void Execute(Graphics *pGfx, RenderGraph& graph) override;
    private:
        ResourceHandle<TinyEngine::BufferResource> bufferhandle;
    };

}