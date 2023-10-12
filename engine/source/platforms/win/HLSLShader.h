#pragma once
#include "Shader.h"
#include <wrl.h>
#include <d3d11.h>
#include <string>
#include <vector>
#include <map>
#include "DirectXGraphics.h"
#include "Shader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace wrl = Microsoft::WRL;
using string = std::string;

class Texture;
class Mesh;
class Material;

// class HLSLShader : public VertexShader, public PixelShader
// {
// public:
//     HLSLShader() = delete;
//     HLSLShader(DirectXGraphics& gfx, const std::wstring& path);
//     HLSLShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint);
//     HLSLShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint, LPCSTR shaderModel);
//     ~HLSLShader() ;

//     ID3D11VertexShader* Get() const;
//     void UpdateConstantBuffer(Material * pMat) override;
//     void UpdateTexture(Material * pMat) override;

//     UINT UpdateVertexBuffers(Mesh& mesh, ID3D11Buffer **& pVertexBuffers,UINT*& strides,UINT*& offsets);
// protected:

// };

class HLSLVertexShader : public VertexShader
{
public:
    HLSLVertexShader() = delete;
    HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path);
    HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint);
    HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint, LPCSTR shaderModel);
    ~HLSLVertexShader() ;

    ID3D11VertexShader* Get() const;
    void SetInputLayout();
    
    void LoadMaterialResource(Material * pMat) override;
    // void UpdateTexture(Material * pMat) override;

    UINT UpdateVertexBuffers(Mesh& mesh, ID3D11Buffer **& pVertexBuffers,UINT*& strides,UINT*& offsets);
private:
    DirectXGraphics& directXGfx;
    wrl::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
    wrl::ComPtr<ID3DBlob> pBlob = nullptr;
    std::unordered_map<int, wrl::ComPtr<ID3D11Buffer>> pConstantBuffersBySlot;
    std::unordered_map<int, wrl::ComPtr<ID3D11ShaderResourceView>> pTextureViewBySlot;
    std::unordered_map<int, wrl::ComPtr<ID3D11SamplerState>> pSamplerStateBySlot;

    std::unordered_map<size_t, wrl::ComPtr<ID3D11Buffer>> pConstantBuffersByID;
    std::unordered_map<size_t, wrl::ComPtr<ID3D11ShaderResourceView>> pTextureViewByID;
    std::unordered_map<size_t, wrl::ComPtr<ID3D11SamplerState>> pSamplerStateByID;

    wrl::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescs;
    std::vector<wrl::ComPtr<ID3D11Buffer>> pVertexBuffers;
    std::vector<ID3D11Buffer*> vertexBufferPtrs;
    std::vector<UINT> strides;
    std::vector<UINT> offsets;
};

class HLSLPixelShader : public PixelShader
{
public:
    HLSLPixelShader() = delete;
    HLSLPixelShader(DirectXGraphics& gfx, const std::wstring& path);
    HLSLPixelShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint);
    HLSLPixelShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint, LPCSTR shaderModel);
    ////move constructor
    //HLSLPixelShader(HLSLPixelShader&& other) noexcept;
    ~HLSLPixelShader() ;
    
    ID3D11PixelShader* Get() const ;
    void LoadMaterialResource(Material * pMat) override;
    // void UpdateTexture(Material * pMat) override;
private:
    DirectXGraphics& directXGfx;
    wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
    wrl::ComPtr<ID3DBlob> pBlob = nullptr;
    std::unordered_map<int, wrl::ComPtr<ID3D11Buffer>> pConstantBuffersBySlot;
    std::unordered_map<int, wrl::ComPtr<ID3D11ShaderResourceView>> pTextureViewBySlot;
    std::unordered_map<int, wrl::ComPtr<ID3D11SamplerState>> pSamplerStateBySlot;

    std::unordered_map<size_t, wrl::ComPtr<ID3D11Buffer>> pConstantBuffersByID;
    std::unordered_map<size_t, wrl::ComPtr<ID3D11ShaderResourceView>> pTextureViewByID;
    std::unordered_map<size_t, wrl::ComPtr<ID3D11SamplerState>> pSamplerStateByID;
};