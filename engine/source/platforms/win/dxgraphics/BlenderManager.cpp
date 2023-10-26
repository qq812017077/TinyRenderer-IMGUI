#include "BlenderManager.h"
#include "Material.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")
#include "DirectXGraphics.h"
#include "effect/Pass.h"

namespace TinyEngine
{
    BlenderManager::BlenderManager()
    {

    }
    BlenderManager::~BlenderManager()
    {

    }

    HRESULT BlenderManager::LoadBlendState(DirectXGraphics* pGfx , const BlendDesc& inputBlendDesc, ID3D11BlendState ** pTargetBlendState)
    {
        HRESULT hr = S_OK;
        int64_t uniqueID = inputBlendDesc.GetHash();
        if(m_BlendStates.find(uniqueID) == m_BlendStates.end())
        {
            D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
            auto& brt = blendDesc.RenderTarget[0];
            brt.BlendEnable = inputBlendDesc.BlendEnable;
            brt.SrcBlend = static_cast<D3D11_BLEND>(inputBlendDesc.SrcBlend);
            brt.DestBlend = static_cast<D3D11_BLEND>(inputBlendDesc.DestBlend);
            brt.BlendOp = static_cast<D3D11_BLEND_OP>(inputBlendDesc.BlendOp);
            brt.SrcBlendAlpha = static_cast<D3D11_BLEND>(inputBlendDesc.SrcBlendAlpha);
            brt.DestBlendAlpha = static_cast<D3D11_BLEND>(inputBlendDesc.DestBlendAlpha);
            brt.BlendOpAlpha = static_cast<D3D11_BLEND_OP>(inputBlendDesc.BlendOpAlpha);
            brt.RenderTargetWriteMask = static_cast<UINT8>(inputBlendDesc.RenderTargetWriteMask);
            hr = pGfx->GetDevice()->CreateBlendState(&blendDesc, &m_BlendStates[uniqueID]);
        }
        *pTargetBlendState = m_BlendStates[uniqueID].Get();
        // switch (pass.renderingMode)
        // {
        //     case ERenderingMode::Opaque:
        //         if(pDefaultOpaqueBlendState == nullptr)
        //         {
        //             D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
        //             hr = pGfx->GetDevice()->CreateBlendState(&blendDesc, &pDefaultOpaqueBlendState);
        //         }
        //         *pTargetBlendState = pDefaultOpaqueBlendState.Get();
        //         break;
            
        //     case ERenderingMode::Transparent:
        //         if(pass.blendFactors != nullptr)
        //         {
        //             wrl::ComPtr<ID3D11BlendState> blendState;
        //             D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
        //             auto& brt = blendDesc.RenderTarget[0];
        //             brt.BlendEnable = TRUE;
        //             brt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
        //             brt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
        //             brt.BlendOp = D3D11_BLEND_OP_ADD;
        //             brt.SrcBlendAlpha = D3D11_BLEND_ZERO;
        //             brt.DestBlendAlpha = D3D11_BLEND_ZERO;
        //             brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        //             brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        //             hr = pGfx->GetDevice()->CreateBlendState(&blendDesc, &blendState);
        //             *pTargetBlendState = blendState.Get();
        //         }
        //         else {
        //             if(pDefaultTransparentBlendState == nullptr)
        //             {
        //                 D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
        //                 auto& brt = blendDesc.RenderTarget[0];
        //                 brt.BlendEnable = TRUE;
        //                 brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        //                 brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        //                 brt.BlendOp = D3D11_BLEND_OP_ADD;
        //                 brt.SrcBlendAlpha = D3D11_BLEND_ZERO;
        //                 brt.DestBlendAlpha = D3D11_BLEND_ZERO;
        //                 brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        //                 brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        //                 hr = pGfx->GetDevice()->CreateBlendState(&blendDesc, &pDefaultTransparentBlendState);
        //             }
        //             *pTargetBlendState = pDefaultTransparentBlendState.Get();
        //         }
        //         break;
        // default:
        //         if(pDefaultOpaqueBlendState == nullptr)
        //         {
        //             D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
        //             hr = pGfx->GetDevice()->CreateBlendState(&blendDesc, &pDefaultOpaqueBlendState);
        //         }
        //         *pTargetBlendState = pDefaultOpaqueBlendState.Get();
        //     break;
        // }
        return hr;
    }
}