#pragma once
#include <d3d11.h>
#include <wrl.h>

#include "dxgraph/dxpasslib/RasterPass.h"
#include "graph/Attachment.h"
#include "directX/DirectXCubeTexture.h"

class DirectXGraphics;


namespace TinyEngine
{
    class Scene;
    class DirectXRenderTarget;
    class DirectXDepthStencil;
    class DitectXDepthArray;
    class CubeRenderTexture;
}
namespace TinyEngine::Graph
{
    class ShadingPass : public RasterPass
    {
    public:
        ShadingPass(std::string name);
        virtual ~ShadingPass() = default;
        
    void BindScene(std::shared_ptr<TinyEngine::Scene> pScene) noexcept override;
    protected:
        void internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph) override;
    private:
        ResourceHandle<TinyEngine::DitectXDepthArray> shadowMapArrayHandle;
        ResourceHandle<TinyEngine::CubeRenderTexture> shadowCubeMapHandle;
        
        TinyEngine::Rendering::DirectXCubeTexture irradianceMap;
        TinyEngine::Rendering::DirectXCubeTexture prefilterMap;
        TinyEngine::Rendering::DirectXTexture pBrdfLutMap;
        
    };
}