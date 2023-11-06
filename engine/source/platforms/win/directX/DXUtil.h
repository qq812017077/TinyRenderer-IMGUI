#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")
//#pragma comment(lib, "d3d11shader.lib")
#include <d3d11shader.h>
#include <vector>
#include <wrl.h>
#include <map>
#include "Mesh.h"
#include "ShaderDesc.h"
#include "TextureFormat.h"
#include "SampleMode.h"
#include "Texture.h"


// ------------------------------
// Shader函数
// CreateShaderFromFile函数:
// [In]csoFileNameInOut 编译好的着色器二进制文件(.cso)，若有指定则优先寻找该文件并读取
// [In]hlslFileName 未编译的着色器源文件(.hlsl)，若csoFileNameInOut为空则使用该文件编译
// [In]entryPoint 着色器入口点
// [In]shaderModel 着色器模型, 如"vs_5_0", "ps_5_0"等
// [Out]ppBlobOut 编译好的着色器二进制文件(.cso)
// ------------------------------
HRESULT CreateShaderFromFile(const WCHAR * csoFileNameInOut, const WCHAR * hlslFileName,
    LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob ** ppBlobOut);

DXGI_FORMAT GetInputElementFormatByMask(BYTE inputParamMask, D3D_REGISTER_COMPONENT_TYPE componentType);


// ------------------------------
// ShaderInfo函数
// ------------------------------
HRESULT GetShaderInfo(const void* shaderBytecode, size_t bytecodeLength,ID3D11ShaderReflection** ppReflection);
// HRESULT GetShaderInfo(const void* shaderBytecode, size_t bytecodeLength, std::vector<UniformVar>* pUniformVars, 
// std::vector<D3D11_INPUT_ELEMENT_DESC>* pLed, std::map<std::string, int> * pTextureSlotMap);
void GetInputLayoutInfo(ID3D11ShaderReflection* pReflection, std::vector<D3D11_INPUT_ELEMENT_DESC>* pLed);

// ------------------------------
// Buffer函数
// ------------------------------
static HRESULT 
CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, D3D11_BIND_FLAG bind_flag, void* pData, 
    UINT bufByteSize, UINT stride, ID3D11Buffer **ppOutputBuffer);
HRESULT CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, void* pData, UINT bufByteSize, ID3D11Buffer** ppOutputBuffer);

HRESULT CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, void* pData, UINT bufByteSize, UINT stride, ID3D11Buffer** ppOutputBuffer);

HRESULT CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, void* pData, UINT bufByteSize, UINT stride, ID3D11Buffer** ppOutputBuffer);

DXGI_FORMAT GetVertexDataFormat(VertexDataType vertexType);
DXGI_FORMAT GetIndexDataFormat(unsigned int indexStride);
UINT GetDataStride(DXGI_FORMAT format);
// ------------------------------
// Texture函数
// ------------------------------
DXGI_FORMAT GetTextureFormat(ETextureFormat textureFormat, bool islinear=false);
UINT GetTextureBindFlag(TextureResDesc::BindFlag bindflag);
D3D11_FILTER GetTextureFilterMode(EFilterMode filterMode);
D3D11_TEXTURE_ADDRESS_MODE GetTextureWrapMode(EWrapMode wrapMode);

HRESULT CreateTexture2DView(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext,  
    Texture* pInputTex, ID3D11ShaderResourceView ** ppOutputTextureView);
HRESULT CreateTexture2DView(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,  
    Texture* pInputTex, ID3D11ShaderResourceView ** ppOutputTextureView);
HRESULT CreateSampler(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice,  Texture * pInputTex, ID3D11SamplerState ** ppOutputSampler);