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
class HLSLVertexShader : public VertexShader
{
public:
    HLSLVertexShader() = delete;
    HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path);
    HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint);
    HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint, LPCSTR shaderModel);
    ~HLSLVertexShader() ;

    ID3D11VertexShader* Get() const;
    bool HasConstantBuffer() const;
    wrl::ComPtr<ID3D11Buffer> GetConstantBuffer() const; 
    bool CreateTexture2D(Texture * pInputTex, ID3D11Texture2D ** ppOutputTexture2D);
    bool CreateSampler(Texture * pInputTex, ID3D11SamplerState ** ppOutputSampler);
    
    void SetInputLayout(Mesh & mesh) override;

private:
    void SetInputElementDescription(Mesh & mesh);
    DirectXGraphics& directXGfx;
    wrl::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
    wrl::ComPtr<ID3DBlob> pBlob = nullptr;
    wrl::ComPtr<ID3D11Buffer> pMaterialConstantBuffer = nullptr;
    wrl::ComPtr<ID3D11InputLayout> pInputLayout;
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescs;
    std::map<std::string, int> textureSlotMap;
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
    bool HasConstantBuffer() const;
    wrl::ComPtr<ID3D11Buffer> GetConstantBuffer() const;
    bool CreateTexture2D(Texture * pInputTex, ID3D11Texture2D ** ppOutputTexture2D);
    bool CreateSampler(Texture * pInputTex, ID3D11SamplerState ** ppOutputSampler);

private:
    DirectXGraphics& directXGfx;
    wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
    wrl::ComPtr<ID3DBlob> pBlob = nullptr;
    wrl::ComPtr<ID3D11Buffer> pMaterialConstantBuffer = nullptr;
    std::map<std::string, int> textureSlotMap;
    
};