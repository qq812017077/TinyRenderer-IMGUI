#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "RasterPass.h"
#include "effect/Pass.h"
#include "dxgraphics/DirectXRenderTarget.h"
#include "DirectXGraphics.h"
#include "effect/Pass.h"
#include "scene/Scene.h"
#include "HLSLShaderHelper.h"
#include "dxgraph/DirectXRenderGraph.h"
#include "geometry/Primitive.h"
#include "directX/DirectXCubeTexture.h"
#include "Exceptions.h"

// #ifndef STB_IMAGE_WRITE_IMPLEMENTATION

// #endif
namespace wrl = Microsoft::WRL;
class DirectXGraphics;

namespace TinyEngine::Graph
{
    class SkyBoxPass : public RasterPass
    {
    public:
        SkyBoxPass(std::string name) : RasterPass(name)
        {
            std::vector<std::string> paths;
            skyboxPass = EffectManager::Get().FindPass("SkyBox/SkyBoxPass");
            skyboxPass.depthStencilDesc.depthMode = EDepthMode::LessEqualPass;
            skyboxPass.rasterDesc.cullMode = ECullMode::Front;
            cubeMesh = Primitive::CreateCubeMesh(3.0f);
        }

        ~SkyBoxPass() override = default;

        void BindScene(std::shared_ptr<TinyEngine::Scene> pScene) noexcept override {
            this->pScene = pScene;
            skyboxTexture.BindCubeMap(this->pScene->p_map_resource->m_skybox_cubemap);
        }

        void internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph) override
        {
            if(!skyboxTexture.HasCreated()) skyboxTexture.CreateResource(pGfx);
            skyboxTexture.BindAsTexture(pGfx, 0);
            pGfx->ApplyPassToMesh(skyboxPass, &cubeMesh);
        }

        void RenderCubeMap(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph, Renderer * pRenderer, int width = 512, int height =512);

        void RenderIrradianceMap(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph, int width = 32, int height = 32);

        std::shared_ptr<CubeTexture> GeneratePrefilterMap(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph, int width = 256, int height = 256, int mipLevels = 5);
    
        
        DirectXCubeTexture skyboxTexture;
    private:
        ShaderPass skyboxPass;
        Mesh cubeMesh;
    };
}