#pragma once
#include "graph/RenderPass.h"
#include "graph/RenderResource.h"

namespace TinyEngine
{
    class DirectXRenderTarget;
    class DirectXDepthStencil;
}

class DirectXGraphics;
namespace TinyEngine::Graph
{
    class DXDefaultRenderGraph;

    class DXRenderPass : public RenderPass
    {
    public:
        DXRenderPass(std::string name) : RenderPass(name) {}
        virtual ~DXRenderPass() = default;
        void Initialize(RenderGraph & graph) override{}
        void Execute(Graphics *pGfx, RenderGraph& graph) override;
        virtual void internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph) = 0;
    };
    // class ResourceHandle;
    class RasterPass : public DXRenderPass
    {
    public:
        RasterPass(std::string name);
        virtual ~RasterPass() = default;
        
        void Execute(Graphics *pGfx, RenderGraph& graph) override;
        virtual void internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph) = 0;
        void Initialize(RenderGraph & graph) override;
    protected:
        ResourceHandle<TinyEngine::DirectXRenderTarget> renderTargetHandle;
        ResourceHandle<TinyEngine::DirectXDepthStencil> depthStencilHandle;
    };
}