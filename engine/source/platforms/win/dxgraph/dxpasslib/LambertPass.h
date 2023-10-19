#pragma once
#include <d3d11.h>
#include <wrl.h>

#include "dxgraph/dxpasslib/RasterPass.h"
#include "graph/Attachment.h"

class DirectXGraphics;


namespace TinyEngine
{
    class Scene;
    class DirectXRenderTarget;
    class DirectXDepthStencil;
}
namespace TinyEngine::Graph
{
    
    class LambertPass : public RasterPass
    {
    public:
        LambertPass(std::string name);
        virtual ~LambertPass() = default;

    protected:
        void internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph) override;
    private:
        ResourceHandle<TinyEngine::DirectXDepthStencil> shadowMapHandle;
    };
}