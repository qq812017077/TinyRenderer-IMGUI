#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>
#include <array>
#include <memory>
#include "Texture.h"


class DirectXGraphics;

namespace TinyEngine::Rendering
{
    class DXTexture
    {
        public:
            virtual void CreateResource(DirectXGraphics* pGfx) = 0;
            virtual void BindAsTexture(DirectXGraphics* pGfx, UINT slot) = 0;
            bool HasCreated() const { return pTextureView != nullptr; }

        protected:
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView{ nullptr };
            Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler{ nullptr };
    };
    class DirectXCubeTexture : public DXTexture
    {
        public:
            void CreateResource(DirectXGraphics* pGfx);
            void BindAsTexture(DirectXGraphics* pGfx, UINT slot);

            // void BindCubeMap(std::array<std::shared_ptr<Texture>, 6>& pTextures);
            void BindCubeMap(std::shared_ptr<CubeTexture> pCubeTex);
        private:
            std::shared_ptr<CubeTexture> pCubeTex{nullptr};
            // std::array<std::shared_ptr<Texture>, 6> textures;
    };

    class DirectXTexture  : public DXTexture
    {
        public:
            void CreateResource(DirectXGraphics* pGfx) override ;
            void BindAsTexture(DirectXGraphics* pGfx, UINT slot) override ;

            void BindTexture(std::shared_ptr<Texture> pTexture);
        private:
            std::shared_ptr<Texture> pTex{nullptr};
    };
}