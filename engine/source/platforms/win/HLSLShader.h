#pragma once
#include "Shader.h"
#include <wrl.h>
#include <d3d11.h>
#include <string>
#include <vector>
#include <map>
#include "DirectXGraphics.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace wrl = Microsoft::WRL;
using string = std::string;

class Texture;
class Mesh;
class HLSLVertexShader : public VertexShader
{
public:
    HLSLVertexShader() = delete;
    HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path);
    HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint);
    HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint, LPCSTR shaderModel);
    ~HLSLVertexShader() ;

    ID3D11VertexShader* Get() const;
    bool CreateTexture2D(Texture * pInputTex, ID3D11Texture2D ** ppOutputTexture2D);
    bool CreateSampler(Texture * pInputTex, ID3D11SamplerState ** ppOutputSampler);
    void SetInputLayout() override;
    

    void UpdateConstantBuffer() override;
    void UpdateTexture();

    UINT UpdateVertexBuffers(Mesh& mesh, ID3D11Buffer **& pVertexBuffers,UINT*& strides,UINT*& offsets);
private:
    DirectXGraphics& directXGfx;
    wrl::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
    wrl::ComPtr<ID3DBlob> pBlob = nullptr;
    std::unordered_map<int, wrl::ComPtr<ID3D11Buffer>> pConstantBuffersBySlot;
    std::unordered_map<int, wrl::ComPtr<ID3D11ShaderResourceView>> pTextureViewBySlot;
    std::unordered_map<int, wrl::ComPtr<ID3D11SamplerState>> pSamplerStateBySlot;

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
    bool CreateTexture2D(Texture * pInputTex, ID3D11Texture2D ** ppOutputTexture2D);
    bool CreateSampler(Texture * pInputTex, ID3D11SamplerState ** ppOutputSampler);
    
    void UpdateConstantBuffer() override;
    void UpdateTexture();
private:
    DirectXGraphics& directXGfx;
    wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
    wrl::ComPtr<ID3DBlob> pBlob = nullptr;
    std::unordered_map<int, wrl::ComPtr<ID3D11Buffer>> pConstantBuffersBySlot;
    std::unordered_map<int, wrl::ComPtr<ID3D11ShaderResourceView>> pTextureViewBySlot;
    std::unordered_map<int, wrl::ComPtr<ID3D11SamplerState>> pSamplerStateBySlot;
};