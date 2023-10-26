#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>
#include "Texture.h"

class DirectXGraphics;

namespace TinyEngine::Rendering
{
    class DirectXCubeTexture : public CubeTexture
    {
        public:
            void CreateResource(DirectXGraphics* pGfx);

            void BindAsTexture(DirectXGraphics* pGfx, UINT slot);
            void LoadFromFiles(std::vector<std::string> paths);
            bool HasCreated() const { return pTexture != nullptr; }
        private:
            Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture{ nullptr };
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView{ nullptr };
            Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler{ nullptr };
    };

    
}