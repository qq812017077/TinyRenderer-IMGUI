#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <optional>
#include <vector>
#include "graphics/RenderTarget.h"

class DirectXGraphics;
class Graphics;
struct TextureResDesc;
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
            DirectXRenderTarget(DirectXGraphics* pGfx, ID3D11Texture2D* pTexture, std::optional<UINT> face = std::nullopt, std::optional<UINT> mipmapLevel = std::nullopt);
            DirectXRenderTarget(DirectXGraphics* pGfx, unsigned int width, unsigned int height, DXGI_FORMAT format= DXGI_FORMAT_R8G8B8A8_UNORM);
            ~DirectXRenderTarget()  = default;

            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView{nullptr};
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView{nullptr};
            
            virtual void Clear(Graphics* pGfx)  override;
            
            void BindAsTexture(Graphics* pGfx, unsigned int slot) override;
            
            static std::shared_ptr<DirectXRenderTarget> Create(Graphics* pGfx, TinyEngine::Graph::ResourceDesc desc);
        private:
            void internalClear(DirectXGraphics* pGfx);

    };

    class CubeRenderTexture : public BufferResource
    {
        public:
            CubeRenderTexture(DirectXGraphics* pGfx, TextureResDesc texDesc);
            void BindAsTexture(Graphics* pGfx, unsigned int slot) { BindAsTexture(reinterpret_cast<DirectXGraphics*>(pGfx), slot); }
            void Clear(Graphics* pGfx) override;
            
            void BindAsTexture(DirectXGraphics* pGfx, UINT slot);
            DirectXRenderTarget * GetFaceBuffer(unsigned int face);
            static std::shared_ptr<CubeRenderTexture> Create(Graphics* pGfx, TinyEngine::Graph::ResourceDesc desc);
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView{ nullptr };
        private:
            std::vector<std::unique_ptr<DirectXRenderTarget>> renderBuffers;
    };

}