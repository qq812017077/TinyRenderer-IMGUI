#pragma once
#include "core/Singleton.h"
#include <wrl.h>
#include <d3d11.h>
#include <d3dcompiler.h>

class Material;
class DirectXGraphics;
namespace wrl = Microsoft::WRL;
namespace TinyEngine
{
    struct ShaderPass;
    class BlenderManager : public Singleton<BlenderManager>
    {
    public:
        BlenderManager();
        ~BlenderManager();

        HRESULT LoadBlendState(DirectXGraphics* pGfx ,  const ShaderPass& pass, ID3D11BlendState ** pTargetBlendState);
    private:

        wrl::ComPtr<ID3D11BlendState> pDefaultTransparentBlendState = nullptr;
        wrl::ComPtr<ID3D11BlendState> pDefaultOpaqueBlendState = nullptr;
    };
}