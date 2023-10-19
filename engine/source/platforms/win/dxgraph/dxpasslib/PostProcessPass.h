#pragma once
#include "dxgraph/dxpasslib/RasterPass.h"
#include <d3d11.h>
#include <wrl.h>
#include "effect/Pass.h"
namespace wrl = Microsoft::WRL;
class DirectXGraphics;

namespace TinyEngine::Graph
{
    class PostProcessPass : public RasterPass
    {
    public:
        PostProcessPass(std::string name);
        virtual ~PostProcessPass() = default;

        void Execute(Graphics *pGfx, RenderGraph& graph) override;
        void internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph) override;

    private:
        ResourceHandle<TinyEngine::DirectXRenderTarget> postHandle;
        ShaderPass maskPass;
        ShaderPass outlinePass;
        ShaderPass fullscreenPass;
    };
}