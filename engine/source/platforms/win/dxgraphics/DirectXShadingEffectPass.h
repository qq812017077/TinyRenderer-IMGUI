#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "effect/EffectPass.h"

namespace wrl = Microsoft::WRL;
class DirectXGraphics;
namespace TinyEngine
{
    /***
     * @brief shading pass
     *  including all lighting part and unlit part
    */
    class DirectXShadingEffectPass : public EffectPassBase
    {
    public:
        DirectXShadingEffectPass();

        ~DirectXShadingEffectPass() override;

        void Apply(Graphics* pGfx, std::shared_ptr<Scene> scene) override
        {
            internalApply(reinterpret_cast<DirectXGraphics*>(pGfx), scene);
        }

    private:
        void internalApply(DirectXGraphics* pGfx, std::shared_ptr<Scene> scene);

        wrl::ComPtr<ID3D11Buffer> pFrameConstantBuffer = nullptr;
        wrl::ComPtr<ID3D11Buffer> pLightingConstantBuffer = nullptr;
    };
}