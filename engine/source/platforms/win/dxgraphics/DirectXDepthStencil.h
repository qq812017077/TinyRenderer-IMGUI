#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <vector>

#include "function/graphics/DepthStencil.h"

class DirectXGraphics;
class Graphics;
struct TextureResDesc;
namespace TinyEngine::Graph
{
    struct ResourceDesc;
}
namespace TinyEngine
{
    class DirectXDepthStencil : public DepthStencil
    {
        public:
            enum class Usage
            {
                DepthStencil,
                ShadowDepth
            };
            DirectXDepthStencil(DirectXGraphics* pGfx, unsigned int width, unsigned int height, Usage usage = Usage::DepthStencil);
            DirectXDepthStencil(DirectXGraphics* pGfx, Microsoft::WRL::ComPtr<ID3D11Texture2D>& pTexture, unsigned int face);
            ~DirectXDepthStencil() = default;

            void BindAsTexture(Graphics* pGfx, unsigned int slot) override;
            void Clear(Graphics* pGfx) override;
            Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState() { return pSamplerState; }
            ID3D11DepthStencilView* Get() { return pDepthStencilView.Get(); }
            static std::shared_ptr<DirectXDepthStencil> Create(Graphics* pGfx, TinyEngine::Graph::ResourceDesc desc);
        private:
            void internalClear(DirectXGraphics* pGfx);
            
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView{nullptr};
            Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState{nullptr};
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView{nullptr};
    };

    class DitectXDepthArray : public DepthStencil
    {
        public:
            DitectXDepthArray(DirectXGraphics* pGfx, TextureResDesc& texDesc);
            void BindAsTexture(Graphics* pGfx, unsigned int slot) override;
            void Clear(Graphics* pGfx) override;
            DirectXDepthStencil * GetIndexBuffer(unsigned int index) {return depthBuffers[index].get();}
            Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState() { return pSamplerState; }

            static std::shared_ptr<DitectXDepthArray> Create(Graphics* pGfx, TinyEngine::Graph::ResourceDesc desc);
        private:
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView{nullptr};
            Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState{nullptr};

            std::vector<std::unique_ptr<DirectXDepthStencil>> depthBuffers;
    };
}