#include "DXUtil.h"
#include "EngineWin.h"

// COM Release
#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }


HRESULT CreateShaderFromFile(const WCHAR * csoFileNameInOut, const WCHAR * hlslFileName,
    LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob ** ppBlobOut)
{
    if ( !csoFileNameInOut || !entryPoint || !shaderModel || !ppBlobOut )
       return E_INVALIDARG;
    
    HRESULT hr = S_OK;

    // Find if has existed compiled shader file
    if (csoFileNameInOut && D3DReadFileToBlob(csoFileNameInOut, ppBlobOut) == S_OK)
    {
        return hr;
    }
    else
    {
        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        // Set the 'D3DCOMPILE_DEBUG' flag to fetch shader debugging information: can improve the debugging experience,
        dwShaderFlags |= D3DCOMPILE_DEBUG;

        // Disable optimizations in the Debug environment to avoid some unreasonable situations
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

        // If specified output file name, output the shader binary information.
        if (csoFileNameInOut)
        {
            return D3DWriteBlobToFile(*ppBlobOut, csoFileNameInOut, FALSE);
        }
    }

    return hr;
}


HRESULT GetShaderInfo(const void* shaderBytecode, size_t bytecodeLength, ID3D11ShaderReflection** ppReflection)
{
    HRESULT hr = D3DReflect(shaderBytecode, bytecodeLength, IID_ID3D11ShaderReflection, (void**)ppReflection);
    return hr;
}
void GetInputLayoutInfo(ID3D11ShaderReflection* pReflection, std::vector<D3D11_INPUT_ELEMENT_DESC>* pLed)
{
    if(pReflection == nullptr) return ;
    pLed->clear();
    HRESULT hr;
    for (UINT i = 0; ; i++)
    {
        D3D11_SIGNATURE_PARAMETER_DESC inputParamDesc;
        hr = pReflection->GetInputParameterDesc(i, &inputParamDesc);
        if (FAILED(hr)) break;
        // 
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
void LoadShaderDescInfo(ID3D11ShaderReflection* pReflection, ShaderDesc * pShaderDesc)
{
    HRESULT hr;
    for(UINT i=0; ; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC bindDesc;
        hr = pReflection->GetResourceBindingDesc(i, &bindDesc);
        if(FAILED(hr)) break;
        switch (bindDesc.Type)
        {
            case D3D_SIT_CBUFFER: // constant buffer
                {
                    ID3D11ShaderReflectionConstantBuffer* constantBuffer = pReflection->GetConstantBufferByName(bindDesc.Name);
                    D3D11_SHADER_BUFFER_DESC bufferDesc;
                    constantBuffer->GetDesc(&bufferDesc);
                    pShaderDesc->AddConstantBufferInfo(bufferDesc.Name, bindDesc.BindPoint, bufferDesc.Size);
                    for (UINT j = 0; j < bufferDesc.Variables; ++j)
                    {
                        ID3D11ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
                        // get variable slot number
                        D3D11_SHADER_VARIABLE_DESC variableDesc;
                        variable->GetDesc(&variableDesc);

                        ID3D11ShaderReflectionType* variableType = variable->GetType();
                        D3D11_SHADER_TYPE_DESC stDesc;
                        variableType->GetDesc(&stDesc);
                        pShaderDesc->AddVariable(bindDesc.BindPoint, variableDesc.Name, variableDesc.StartOffset, variableDesc.Size);
                    }
                }
                break;
            case D3D_SIT_TEXTURE: // texture
                {
                    pShaderDesc->AddTexture(bindDesc.BindPoint, bindDesc.Name);
                }
                break;
            case D3D_SIT_SAMPLER: // sampler
                {
                    SamplerInfo samplerInfo;
                    samplerInfo.slot = bindDesc.BindPoint;
                    samplerInfo.name = bindDesc.Name;
                    pShaderDesc->AddSamplerInfo(samplerInfo);
                }
                break;
        }
    }
}



// void GetTextureInfo(ID3D11ShaderReflection* pReflection, D3D11_SHADER_DESC& shaderDesc, std::map<std::string, int> * pTextureSlotMap)
// {
//     if(pReflection == nullptr) return ;
//     pTextureSlotMap->clear();
//     // collect all texture slots and names
//     for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
//     {
//         D3D11_SHADER_INPUT_BIND_DESC bindDesc;
//         pReflection->GetResourceBindingDesc(i, &bindDesc);
//         if (bindDesc.Type == D3D_SIT_TEXTURE)
//         {
//             // texture slots
//             pTextureSlotMap->insert({ bindDesc.Name, bindDesc.BindPoint });
//         }
//         else if (bindDesc.Type == D3D_SIT_SAMPLER)
//         {
//             // sampler slots
//         }
//     }
// }

// HRESULT GetShaderInfo(const void* shaderBytecode, size_t bytecodeLength, std::vector<UniformVar>* pUniformVars, 
// std::vector<D3D11_INPUT_ELEMENT_DESC>* pLed, std::map<std::string, int> * pTextureSlotMap)
// {
//     Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflection = nullptr;
//     D3D11_SHADER_DESC shaderDesc;
//     HRESULT hr = GetShaderInfo(shaderBytecode, bytecodeLength, &pReflection, &shaderDesc);
//     if (FAILED(hr)) return hr;
//     if(pUniformVars != nullptr) GetUniformsInfo(pReflection.Get(), shaderDesc, pUniformVars);
//     if(pLed != nullptr) GetInputLayoutInfo(pReflection.Get(), shaderDesc, pLed);
//     if(pTextureSlotMap != nullptr) GetTextureInfo(pReflection.Get(), shaderDesc, pTextureSlotMap);
//     return hr;
// }

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
static HRESULT CreateBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, D3D11_BIND_FLAG bind_flag, void* pData, UINT bufByteSize, UINT stride, ID3D11Buffer **ppOutputBuffer)
{
    D3D11_BUFFER_DESC cbd = {};
    ZeroMemory(&cbd, sizeof(cbd));
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

DXGI_FORMAT GetTextureFormat(ETextureFormat textureFormat, bool islinear)
{
    if(textureFormat == ETextureFormat::ARGB32)
        return islinear? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    else if(textureFormat == ETextureFormat::ARGB4444)
    {
        return DXGI_FORMAT_B4G4R4A4_UNORM;
    }

    return islinear? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
}

D3D11_FILTER GetTextureFilterMode(EFilterMode filterMode)
{
    switch (filterMode)
    {
        case EFilterMode::Point:
            return D3D11_FILTER_MIN_MAG_MIP_POINT;
        case EFilterMode::Bilinear:
        case EFilterMode::Trilinear:
            return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            // return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            // return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        case EFilterMode::Anisotropic:
            return D3D11_FILTER_ANISOTROPIC;
    }
    return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
}
D3D11_TEXTURE_ADDRESS_MODE GetTextureWrapMode(EWrapMode wrapMode)
{
    switch (wrapMode)
    {
        case EWrapMode::Repeat:
            return D3D11_TEXTURE_ADDRESS_WRAP;
        case EWrapMode::Clamp:
            return D3D11_TEXTURE_ADDRESS_CLAMP;
        case EWrapMode::Mirror:
            return D3D11_TEXTURE_ADDRESS_MIRROR;
        case EWrapMode::MirrorOnce:
            return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
        case EWrapMode::Border:
            return D3D11_TEXTURE_ADDRESS_BORDER;
    }
    return D3D11_TEXTURE_ADDRESS_WRAP;
}