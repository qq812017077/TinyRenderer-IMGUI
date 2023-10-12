#include "StencilManager.h"
#include "Material.h"
#include "effect/Pass.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")
#include "DirectXGraphics.h"
namespace TinyEngine
{
    StencilManager::StencilManager()
    {
        
    }
    StencilManager::~StencilManager()
    {

    }
    HRESULT StencilManager::LoadStencilState(DirectXGraphics* pGfx , const DepthStencilDesc& depthStencilDesc, ID3D11DepthStencilState ** pTargetBlendState)
    {
        HRESULT hr = S_OK;
        size_t uniqueID = 0;
        uniqueID |= (size_t)(depthStencilDesc.depthMode) << 8;
        uniqueID |= (size_t)(depthStencilDesc.stencilMode);
        if(m_StencilStates.find(uniqueID) == m_StencilStates.end())
        {
            D3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
            switch (depthStencilDesc.stencilMode)
            {
            case TinyEngine::Rendering::EStencilMode::WriteMask:
                /* code */
                desc.DepthEnable = FALSE;
                desc.StencilEnable = TRUE;
                desc.StencilWriteMask = 0xFF;
                desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
                desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
                break;
            case TinyEngine::Rendering::EStencilMode::ReadMask:
                desc.DepthEnable = FALSE;
                desc.StencilEnable = TRUE;
                desc.StencilReadMask = 0xFF;
                desc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
                desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
                hr = pGfx->GetDevice()->CreateDepthStencilState(&desc, &pReadDepthStencilState);
                break;
            default:
                break;
            }
            hr = pGfx->GetDevice()->CreateDepthStencilState(&desc, &m_StencilStates[uniqueID]);

        }

        *pTargetBlendState = m_StencilStates[uniqueID].Get();

        return hr;
    }
    HRESULT StencilManager::LoadStencilState(DirectXGraphics* pGfx , const TinyEngine::Rendering::EStencilMode mode, ID3D11DepthStencilState ** pTargetBlendState)
    {
        HRESULT hr = S_OK;
        switch (mode)
        {
            case TinyEngine::Rendering::EStencilMode::Off:
            {
                if(pDefaultDepthStencilState == nullptr)
                {
                    D3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
                    hr = pGfx->GetDevice()->CreateDepthStencilState(&desc, &pDefaultDepthStencilState);
                }
                *pTargetBlendState = pDefaultDepthStencilState.Get();
                break;
            }
            case TinyEngine::Rendering::EStencilMode::WriteMask:
            {   
                if(pWriteDepthStencilState == nullptr)
                {
                    D3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
                    desc.DepthEnable = FALSE;
                    desc.StencilEnable = TRUE;
                    desc.StencilWriteMask = 0xFF;
                    desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
                    desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
                    hr = pGfx->GetDevice()->CreateDepthStencilState(&desc, &pWriteDepthStencilState);
                }
                *pTargetBlendState = pWriteDepthStencilState.Get();
                break;
            }
            case TinyEngine::Rendering::EStencilMode::ReadMask:
            {   
                if(pReadDepthStencilState == nullptr)
                {
                    D3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
                    desc.DepthEnable = FALSE;
                    desc.StencilEnable = TRUE;
                    desc.StencilReadMask = 0xFF;
                    desc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
                    desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
                    hr = pGfx->GetDevice()->CreateDepthStencilState(&desc, &pReadDepthStencilState);
                }
                *pTargetBlendState = pReadDepthStencilState.Get();
                break;
            }
        default:
            break;
        }
        return hr;
    }

    
}