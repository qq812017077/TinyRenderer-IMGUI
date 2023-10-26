#pragma once
#include "core/Singleton.h"
#include <wrl.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <unordered_map>
class Material;
class DirectXGraphics;
namespace wrl = Microsoft::WRL;
namespace TinyEngine
{
    struct BlendDesc;
    class BlenderManager : public Singleton<BlenderManager>
    {
    public:
        BlenderManager();
        ~BlenderManager();

        HRESULT LoadBlendState(DirectXGraphics* pGfx ,  const BlendDesc& BlendDesc, ID3D11BlendState ** pTargetBlendState);
    private:
        std::unordered_map<int64_t, wrl::ComPtr<ID3D11BlendState>> m_BlendStates;
        // wrl::ComPtr<ID3D11BlendState> pDefaultTransparentBlendState = nullptr;
        // wrl::ComPtr<ID3D11BlendState> pDefaultOpaqueBlendState = nullptr;
    };
}