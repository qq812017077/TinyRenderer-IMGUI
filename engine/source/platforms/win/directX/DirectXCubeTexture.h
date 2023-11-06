#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>
#include <array>
#include <memory>
#include "Texture.h"
#include  "graphics/BufferResource.h"

class Graphics;
class DirectXGraphics;

namespace TinyEngine::Rendering
{
    class DXTexture : public BufferResource
    {
        public:
            DXTexture() : BufferResource(nullptr, 0, 0) {}
            virtual void CreateResource(DirectXGraphics* pGfx) = 0;
            void BindAsTexture(Graphics* pGfx, UINT slot) override { internalBindAsTexture(reinterpret_cast<DirectXGraphics*>(pGfx), slot); }
            void Clear(Graphics* pGfx) override { }

            bool HasCreated() const { return pTextureView != nullptr; }

        protected:
            virtual void internalBindAsTexture(DirectXGraphics* pGfx, UINT slot) = 0;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView{ nullptr };
            Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler{ nullptr };
    };


    class DirectXCubeTexture : public DXTexture
    {
        public:
            void CreateResource(DirectXGraphics* pGfx);
            void BindCubeMap(std::shared_ptr<CubeTexture> pCubeTex);
        private:
            void internalBindAsTexture(DirectXGraphics* pGfx, UINT slot) override;
            std::shared_ptr<CubeTexture> pCubeTex{nullptr};
            // std::array<std::shared_ptr<Texture>, 6> textures;
    };

    class DirectXTexture  : public DXTexture
    {
        public:
            void CreateResource(DirectXGraphics* pGfx) override ;

            void BindTexture(std::shared_ptr<Texture> pTexture);
        private:
            void internalBindAsTexture(DirectXGraphics* pGfx, UINT slot) override;
            std::shared_ptr<Texture> pTex{nullptr};
    };
}