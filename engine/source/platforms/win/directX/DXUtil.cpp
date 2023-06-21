#include "DXUtil.h"
#include "EngineWin.h"
// 安全COM组件释放宏
#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }


HRESULT CreateShaderFromFile(const WCHAR * csoFileNameInOut, const WCHAR * hlslFileName,
    LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob ** ppBlobOut)
{
    if ( !csoFileNameInOut || !entryPoint || !shaderModel || !ppBlobOut )
       return E_INVALIDARG;
    
    HRESULT hr = S_OK;

    // 寻找是否有已经编译好的顶点着色器
    if (csoFileNameInOut && D3DReadFileToBlob(csoFileNameInOut, ppBlobOut) == S_OK)
    {
        return hr;
    }
    else
    {
        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        // 设置 D3DCOMPILE_DEBUG 标志用于获取着色器调试信息。该标志可以提升调试体验，
        // 但仍然允许着色器进行优化操作
        dwShaderFlags |= D3DCOMPILE_DEBUG;

        // 在Debug环境下禁用优化以避免出现一些不合理的情况
        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
        ID3DBlob* errorBlob = nullptr;
        hr = D3DCompileFromFile(hlslFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
            dwShaderFlags, 0, ppBlobOut, &errorBlob);
        if (FAILED(hr))
        {
            if (errorBlob != nullptr)
            {
                const char* errorMsg = reinterpret_cast<const char*>(errorBlob->GetBufferPointer());
                OutputDebugStringA(errorMsg);
                MessageBox(nullptr, errorMsg, "Shader Compilation Error", MB_RETRYCANCEL);
            }
            SAFE_RELEASE(errorBlob);
            return hr;
        }

        // 若指定了输出文件名，则将着色器二进制信息输出
        if (csoFileNameInOut)
        {
            return D3DWriteBlobToFile(*ppBlobOut, csoFileNameInOut, FALSE);
        }
    }

    return hr;
}



HRESULT GetShaderInfo(const void* shaderBytecode, size_t bytecodeLength, std::vector<UniformVar>* pUniformVars)
{
    return GetShaderInfo(shaderBytecode, bytecodeLength, pUniformVars, nullptr);
}

HRESULT GetShaderInfo(const void* shaderBytecode, size_t bytecodeLength, std::vector<UniformVar>* pUniformVars, std::vector<D3D11_INPUT_ELEMENT_DESC>* pLed)
{
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflection = nullptr;
    HRESULT hr = D3DReflect(shaderBytecode, bytecodeLength, IID_ID3D11ShaderReflection, &pReflection);
    if (FAILED(hr))
    {
        return hr;
    }
    D3D11_SHADER_DESC shaderDesc;
    pReflection->GetDesc(&shaderDesc);

    if(pUniformVars != nullptr)
    {
        pUniformVars->clear();
        for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
        {
            ID3D11ShaderReflectionConstantBuffer* constantBuffer = pReflection->GetConstantBufferByIndex(i);

            D3D11_SHADER_BUFFER_DESC bufferDesc;
            constantBuffer->GetDesc(&bufferDesc);

            for (UINT j = 0; j < bufferDesc.Variables; ++j)
            {
                ID3D11ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);

                D3D11_SHADER_VARIABLE_DESC variableDesc;
                variable->GetDesc(&variableDesc);
                pUniformVars->push_back(
                    {variableDesc.Name,
                    variableDesc.Size}
                    );
            }
        }
    }

    if(pLed != nullptr)
    {
        pLed->clear();
        for (UINT i = 0; i < shaderDesc.InputParameters; i++)
        {
            D3D11_SIGNATURE_PARAMETER_DESC inputParamDesc;
            pReflection->GetInputParameterDesc(i, &inputParamDesc);

            // 在这里处理 inputParamDesc 中的信息
            auto len = strlen(inputParamDesc.SemanticName);
            D3D11_INPUT_ELEMENT_DESC inputElementDesc;
            inputElementDesc.SemanticName = new char[len + 1];
            strcpy_s(const_cast<char*>(inputElementDesc.SemanticName), len + 1, inputParamDesc.SemanticName);
            inputElementDesc.SemanticIndex = inputParamDesc.SemanticIndex;
            inputElementDesc.Format = GetInputElementFormatByMask(inputParamDesc.Mask, inputParamDesc.ComponentType);
            inputElementDesc.InputSlot = 0;
            inputElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            inputElementDesc.InstanceDataStepRate = 0;
            pLed->push_back(inputElementDesc);
        }
    }
    return hr;
}

static HRESULT CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, D3D11_BIND_FLAG bind_flag, void* pData, UINT bufByteSize, UINT stride, ID3D11Buffer **ppOutputBuffer)
{
    D3D11_BUFFER_DESC cbd = {};
    cbd.ByteWidth = bufByteSize;
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.BindFlags = bind_flag;
    cbd.CPUAccessFlags = 0u;
    cbd.MiscFlags = 0u;
    cbd.StructureByteStride = stride;
    switch(bind_flag)
    {
        case D3D11_BIND_CONSTANT_BUFFER:
            cbd.Usage = D3D11_USAGE_DYNAMIC;
            cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            break;
        case D3D11_BIND_VERTEX_BUFFER:
            break;
        case D3D11_BIND_INDEX_BUFFER:
            break; 
    }
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = pData;
    return pDevice->CreateBuffer(&cbd, &csd, ppOutputBuffer);
}

HRESULT CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, void* pData, UINT bufByteSize, ID3D11Buffer** ppOutputBuffer)
{
    return CreateBuffer(pDevice, D3D11_BIND_CONSTANT_BUFFER, pData, bufByteSize, 0u, ppOutputBuffer);
}

HRESULT CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, void* pData, UINT bufByteSize, UINT stride, ID3D11Buffer** ppOutputBuffer)
{
    return CreateBuffer(pDevice, D3D11_BIND_VERTEX_BUFFER, pData, bufByteSize, stride, ppOutputBuffer);
}

HRESULT CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, void* pData, UINT bufByteSize, UINT stride, ID3D11Buffer** ppOutputBuffer)
{
    return CreateBuffer(pDevice, D3D11_BIND_INDEX_BUFFER, pData, bufByteSize, stride, ppOutputBuffer);
}

DXGI_FORMAT GetInputElementFormatByMask(BYTE inputParamMask, D3D_REGISTER_COMPONENT_TYPE componentType)
{
    switch(inputParamMask)
    {
        case 0x1:
            switch(componentType)
            {
                case D3D_REGISTER_COMPONENT_UINT32:
                    return DXGI_FORMAT_R32_UINT;
                case D3D_REGISTER_COMPONENT_SINT32:
                    return DXGI_FORMAT_R32_SINT;
                case D3D_REGISTER_COMPONENT_FLOAT32:
                    return DXGI_FORMAT_R32_FLOAT;
            }
        case 0x3:
            switch(componentType)
            {
                case D3D_REGISTER_COMPONENT_UINT32:
                    return DXGI_FORMAT_R32G32_UINT;
                case D3D_REGISTER_COMPONENT_SINT32:
                    return DXGI_FORMAT_R32G32_SINT;
                case D3D_REGISTER_COMPONENT_FLOAT32:
                    return DXGI_FORMAT_R32G32_FLOAT;
            }
        case 0x7:
            switch(componentType)
            {
                case D3D_REGISTER_COMPONENT_UINT32:
                    return DXGI_FORMAT_R32G32B32_UINT;
                case D3D_REGISTER_COMPONENT_SINT32:
                    return DXGI_FORMAT_R32G32B32_SINT;
                case D3D_REGISTER_COMPONENT_FLOAT32:
                    return DXGI_FORMAT_R32G32B32_FLOAT;
            }
        case 0xF:
            switch(componentType)
            {
                case D3D_REGISTER_COMPONENT_UINT32:
                    return DXGI_FORMAT_R32G32B32A32_UINT;
                case D3D_REGISTER_COMPONENT_SINT32:
                    return DXGI_FORMAT_R32G32B32A32_SINT;
                case D3D_REGISTER_COMPONENT_FLOAT32:
                    return DXGI_FORMAT_R32G32B32A32_FLOAT;
            }
    }
    return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT GetVertexDataFormat(VertexDataType vertexType)
{
    switch(vertexType)
    {
        case VertexDataType::Position:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case VertexDataType::Position2D:
            return DXGI_FORMAT_R32G32_FLOAT;
        
        case VertexDataType::Normal:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case VertexDataType::Color:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case VertexDataType::TexCoord:
            return DXGI_FORMAT_R32G32_FLOAT;
        case VertexDataType::Tangent:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case VertexDataType::Binormal:
            return DXGI_FORMAT_R32G32B32_FLOAT;
    }
    return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT GetIndexDataFormat(unsigned int indexStride)
{
    if (indexStride == 4)
        return DXGI_FORMAT_R32_UINT;
    else if (indexStride == 2)
        return DXGI_FORMAT_R16_UINT;
    return DXGI_FORMAT_R16_UINT;
}