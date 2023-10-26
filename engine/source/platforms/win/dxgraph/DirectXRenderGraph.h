#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "graph/RenderGraph.h"
// #include "dxgraphics/DirectXRenderTarget.h"

class DirectXGraphics;

namespace wrl = Microsoft::WRL;
namespace TinyEngine
{
    class DirectXRenderTarget;
    class DirectXDepthStencil;
    class DepthCubeTexture;
}
namespace TinyEngine::Graph
{
    class DXDefaultRenderGraph : public RenderGraph
    {
    public:
        DXDefaultRenderGraph(DirectXGraphics * pGfx);
        ~DXDefaultRenderGraph() override;

        void Resize(DirectXGraphics * pGfx);
        wrl::ComPtr<ID3D11Buffer>& GetFrameConstantBuffer() { return pFrameConstantBuffer; }
        wrl::ComPtr<ID3D11Buffer>& GetLightingConstantBuffer() { return pLightingConstantBuffer; }
    private:
        DirectXGraphics * pGfx;
        TinyEngine::Graph::ResourceHandle<TinyEngine::DirectXRenderTarget> mainRTHandle;
        TinyEngine::Graph::ResourceHandle<TinyEngine::DirectXRenderTarget> effectRTHandle;
        TinyEngine::Graph::ResourceHandle<TinyEngine::DirectXRenderTarget> backBufferHandle;
        TinyEngine::Graph::ResourceHandle<TinyEngine::DirectXDepthStencil> depthStencilHandle;
        TinyEngine::Graph::ResourceHandle<TinyEngine::DirectXDepthStencil> shadowMapHandle;
        TinyEngine::Graph::ResourceHandle<TinyEngine::DepthCubeTexture> shadowCubeHandle;

        
        wrl::ComPtr<ID3D11Buffer> pFrameConstantBuffer = nullptr;
        wrl::ComPtr<ID3D11Buffer> pLightingConstantBuffer = nullptr;
    };
}