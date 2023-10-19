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
            for(int i = 0 ; i < 6; i++)
                paths.emplace_back("res/images/skybox/" + std::to_string(i) + ".png");
            skyboxTexture.LoadFromFiles(paths);
            skyboxPass = EffectManager::Get().FindPass("SkyBox/SkyBoxPass");
            skyboxPass.depthStencilDesc.depthMode = EDepthMode::DepthFirst;
            skyboxPass.rasterDesc.cullMode = ECullMode::Front;
            cubeMesh = Primitive::CreateCubeMesh(3.0f);
        }

        virtual ~SkyBoxPass() = default;

        void internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph) override
        {
            if(!skyboxTexture.HasCreated()) skyboxTexture.CreateResource(pGfx);
            skyboxTexture.BindAsTexture(pGfx, 0);
            pGfx->ApplyPassToMesh(skyboxPass, &cubeMesh);
        }

    private:
        ShaderPass skyboxPass;
        DirectXCubeTexture skyboxTexture;

        Mesh cubeMesh;
    };
}