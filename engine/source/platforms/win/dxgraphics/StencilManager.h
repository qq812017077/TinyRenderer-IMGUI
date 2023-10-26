#pragma once
#include "core/Singleton.h"
#include <wrl.h>
#include <d3d11.h>
#include <unordered_map>
#include <d3dcompiler.h>
#include "effect/Stencil.h"
class Material;
class DirectXGraphics;
namespace wrl = Microsoft::WRL;
namespace TinyEngine
{
    struct DepthStencilDesc;
    class StencilManager : public Singleton<StencilManager>
    {
    public:
        StencilManager();
        ~StencilManager();

        HRESULT LoadStencilState(DirectXGraphics* pGfx , const DepthStencilDesc& desc, ID3D11DepthStencilState ** pTargetBlendState);
        HRESULT LoadStencilState(DirectXGraphics* pGfx , const TinyEngine::Rendering::EStencilMode mode,
         ID3D11DepthStencilState ** pTargetBlendState);
    private:
        std::unordered_map<int64_t, wrl::ComPtr<ID3D11DepthStencilState>> m_StencilStates;
        wrl::ComPtr<ID3D11DepthStencilState> pDefaultDepthStencilState = nullptr;
        wrl::ComPtr<ID3D11DepthStencilState> pWriteDepthStencilState = nullptr;
        wrl::ComPtr<ID3D11DepthStencilState> pReadDepthStencilState = nullptr;
    };
}