#include "RenderStates.h"
#include "Exceptions.h"
#include "DirectXGraphics.h"

namespace TinyEngine
{
    using namespace Microsoft::WRL;

    ComPtr<ID3D11RasterizerState> RenderStates::RSWireframe = nullptr;  
    ComPtr<ID3D11RasterizerState> RenderStates::RSNoCull = nullptr;     
    ComPtr<ID3D11SamplerState> RenderStates::SSLinearWrap = nullptr;    
    ComPtr<ID3D11SamplerState> RenderStates::SSAnistropicWrap = nullptr;
    ComPtr<ID3D11BlendState> RenderStates::BSNoColorWrite = nullptr;    
    ComPtr<ID3D11BlendState> RenderStates::BSTransparent = nullptr;     
    ComPtr<ID3D11BlendState> RenderStates::BSAlphaToCoverage = nullptr; 

    bool RenderStates::IsInit()
    {
        return RSWireframe != nullptr;
    }
    void RenderStates::InitAll(DirectXGraphics& gfx)
    {
        if (IsInit()) return;

        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

        #ifndef NDEBUG
            auto & infoManager = gfx.GetInfoManager();
        #endif
        
        HRESULT hr;
        auto device = gfx.GetDevice();
        // wireframe
        rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
        rasterizerDesc.CullMode = D3D11_CULL_NONE;
        rasterizerDesc.FrontCounterClockwise = false;
        rasterizerDesc.DepthClipEnable = true;
        GFX_THROW_INFO(device->CreateRasterizerState(&rasterizerDesc, RSWireframe.GetAddressOf()));

        // no cull
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_NONE;
        rasterizerDesc.FrontCounterClockwise = false;
        rasterizerDesc.DepthClipEnable = true;
        GFX_THROW_INFO(device->CreateRasterizerState(&rasterizerDesc, RSNoCull.GetAddressOf()));

        // ************** sampler states **************
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory(&sampDesc, sizeof(sampDesc));

        // linear wrap
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        GFX_THROW_INFO(device->CreateSamplerState(&sampDesc, SSLinearWrap.GetAddressOf()));

        // anistropic wrap
        sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MaxAnisotropy = 4;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        GFX_THROW_INFO(device->CreateSamplerState(&sampDesc, SSAnistropicWrap.GetAddressOf()));
        
        // ***********blend states***********
        D3D11_BLEND_DESC blendDesc;
        ZeroMemory(&blendDesc, sizeof(blendDesc));
        auto& rtDesc = blendDesc.RenderTarget[0];
        // Alpha-To-Coverage
        blendDesc.AlphaToCoverageEnable = true;
        blendDesc.IndependentBlendEnable = false;
        rtDesc.BlendEnable = false;
        rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        GFX_THROW_INFO(device->CreateBlendState(&blendDesc, BSAlphaToCoverage.GetAddressOf()));

        // Transparent
        // Color = SrcAlpha * SrcColor + (1 - SrcAlpha) * DestColor 
        // Alpha = SrcAlpha
        blendDesc.AlphaToCoverageEnable = false;
        blendDesc.IndependentBlendEnable = false;
        rtDesc.BlendEnable = true;
        rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        rtDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
        rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
        rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
        rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

        GFX_THROW_INFO(device->CreateBlendState(&blendDesc, BSTransparent.GetAddressOf()));
        
        // NoColorWrite
        // Color = DestColor
        // Alpha = DestAlpha
        rtDesc.SrcBlend = D3D11_BLEND_ZERO;
        rtDesc.DestBlend = D3D11_BLEND_ONE;
        rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
        rtDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
        rtDesc.DestBlendAlpha = D3D11_BLEND_ONE;
        rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        GFX_THROW_INFO(device->CreateBlendState(&blendDesc, BSNoColorWrite.GetAddressOf()));
    }
}