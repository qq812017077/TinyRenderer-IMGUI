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
#include "Mesh.h"
#include "UniformVar.h"

// ------------------------------
// CreateShaderFromFile函数
// ------------------------------
// [In]csoFileNameInOut 编译好的着色器二进制文件(.cso)，若有指定则优先寻找该文件并读取
// [In]hlslFileName     着色器代码，若未找到着色器二进制文件则编译着色器代码
// [In]entryPoint       入口点(指定开始的函数)
// [In]shaderModel      着色器模型，格式为"*s_5_0"，*可以为c,d,g,h,p,v之一
// [Out]ppBlobOut       输出着色器二进制信息
HRESULT CreateShaderFromFile(const WCHAR * csoFileNameInOut, const WCHAR * hlslFileName,
    LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob ** ppBlobOut);

DXGI_FORMAT GetInputElementFormatByMask(BYTE inputParamMask, D3D_REGISTER_COMPONENT_TYPE componentType);

HRESULT GetShaderInfo(const void* shaderBytecode, size_t bytecodeLength, std::vector<UniformVar>* pUniformVars, std::vector<D3D11_INPUT_ELEMENT_DESC>* pLed);

static HRESULT 
CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, D3D11_BIND_FLAG bind_flag, void* pData, 
    UINT bufByteSize, UINT stride, ID3D11Buffer **ppOutputBuffer);
HRESULT CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, void* pData, UINT bufByteSize, ID3D11Buffer** ppOutputBuffer);

HRESULT CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, void* pData, UINT bufByteSize, UINT stride, ID3D11Buffer** ppOutputBuffer);

HRESULT CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, void* pData, UINT bufByteSize, UINT stride, ID3D11Buffer** ppOutputBuffer);

DXGI_FORMAT GetVertexDataFormat(VertexDataType vertexType);
DXGI_FORMAT GetIndexDataFormat(unsigned int indexStride);