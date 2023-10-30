#pragma once
#include "dxgraph/dxpasslib/RasterPass.h"
#include <d3d11.h>
#include <wrl.h>
#include "effect/Pass.h"
#include "Mesh.h"
#include "scene/Scene.h"
#include "geometry/Primitive.h"
#include "HLSLShaderHelper.h"
#include "DirectXGraphics.h"
#include "dxgraph/DirectXRenderGraph.h"

namespace wrl = Microsoft::WRL;
class DirectXGraphics;


namespace TinyEngine::Graph
{
    class HubPass : public RasterPass
    {
    public:
        HubPass(std::string name) : RasterPass(name){
            wireframePass = ShaderPass::Get("WireframePass", "shaders/SimpleVertexShader.hlsl", "shaders/UnlitPixelShader.hlsl");
            wireframePass.rasterDesc.cullMode = ECullMode::Off;
            wireframeMesh = Primitive::CreateCameraWireframeMesh();
        }

        virtual ~HubPass() = default;

        void internalExecute(DirectXGraphics* pGfx, DXDefaultRenderGraph& graph) override
        {
            // draw camera frustum
            auto &helper = HLSLShaderHelper::Get();
            pGfx->UpdateCBuffer(graph.GetFrameConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerFrameCBufSlot));
            pGfx->UpdateCBuffer(pGfx->GetObjectConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerDrawCBufSlot));
            
            auto selectedCam = pScene->selectedCamera;
            if(selectedCam && selectedCam != pScene->m_main_camera)
            {
                auto worldMatrix = selectedCam->pTransform->GetWorldMatrix();
                auto frustumMesh = selectedCam->GetFrustumMesh();
                helper.SetGlobalMatrix("g_World", worldMatrix);
                helper.SetGlobalMatrix("g_WorldInv", worldMatrix.Inverse());
                pGfx->UpdateCBuffer(pGfx->GetObjectConstantBuffer(), helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerDrawCBufSlot));
                pGfx->ApplyPassToMesh(wireframePass, &wireframeMesh, Graphics::EDrawMode::LineList);
                pGfx->ApplyPassToMesh(wireframePass, frustumMesh, Graphics::EDrawMode::LineList);
            }
        }

    private:
        ShaderPass wireframePass;
        Mesh wireframeMesh;
    };
}