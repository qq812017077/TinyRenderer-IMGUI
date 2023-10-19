#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include "graphics/RenderTarget.h"

class DirectXGraphics;
class Graphics;
namespace TinyEngine::Graph
{
    struct ResourceDesc;
}
namespace TinyEngine
{
    class DirectXDepthStencil;
    class   DirectXRenderTarget : public RenderTarget
    {
        public:
            DirectXRenderTarget(DirectXGraphics* pGfx, ID3D11Texture2D* pTexture);
            DirectXRenderTarget(DirectXGraphics* pGfx, unsigned int width, unsigned int height);
            ~DirectXRenderTarget()  = default;

            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView{nullptr};
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView{nullptr};
            
            virtual void Clear(Graphics* pGfx)  override;
            
            void BindAsTexture(Graphics* pGfx, unsigned int slot) override;
            
            static std::shared_ptr<DirectXRenderTarget> Create(Graphics* pGfx, TinyEngine::Graph::ResourceDesc desc);
        private:
            void internalClear(DirectXGraphics* pGfx);

    };

}