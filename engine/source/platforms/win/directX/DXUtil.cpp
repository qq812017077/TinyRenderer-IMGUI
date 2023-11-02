#include "DXUtil.h"
#include "EngineWin.h"
#include "Texture.h"
#include "Exceptions.h"
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
        inputElementDesc.InputSlot = i;
        inputElementDesc.AlignedByteOffset = 0;
        inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputElementDesc.InstanceDataStepRate = 0;
        pLed->push_back(inputElementDesc);
    }
}



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
            cbd.Usage = D3D11_USAGE_DEFAULT; // default means
            
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

UINT GetDataStride(DXGI_FORMAT format)
{
    switch (format)
    {
        case DXGI_FORMAT_R32G32B32A32_FLOAT: return 16;
        case DXGI_FORMAT_R32G32B32_FLOAT: return 12;
        case DXGI_FORMAT_R32G32_FLOAT: return 8;
        case DXGI_FORMAT_R32_FLOAT: return 4;
        case DXGI_FORMAT_R8G8B8A8_UNORM: return 4;
        case DXGI_FORMAT_R8G8B8A8_UINT: return 4;
        case DXGI_FORMAT_R8G8B8A8_SNORM: return 4;
        case DXGI_FORMAT_R8G8B8A8_SINT: return 4;
        case DXGI_FORMAT_R16G16B16A16_FLOAT: return 8;
        case DXGI_FORMAT_R16G16B16A16_UNORM: return 8;
        case DXGI_FORMAT_R16G16B16A16_UINT: return 8;
        case DXGI_FORMAT_R16G16B16A16_SNORM: return 8;
        case DXGI_FORMAT_R16G16B16A16_SINT: return 8;
        case DXGI_FORMAT_R32G32B32A32_UINT: return 16;
        case DXGI_FORMAT_R32G32B32A32_SINT: return 16;
        case DXGI_FORMAT_R32G32B32_UINT: return 12;
        case DXGI_FORMAT_R32G32B32_SINT: return 12;
        case DXGI_FORMAT_R16G16_FLOAT: return 4;
        case DXGI_FORMAT_R16G16_UNORM: return 4;
        case DXGI_FORMAT_R16G16_UINT: return 4;
        case DXGI_FORMAT_R16G16_SNORM: return 4;
        case DXGI_FORMAT_R16G16_SINT: return 4;
        case DXGI_FORMAT_R32G32_UINT: return 8;
        case DXGI_FORMAT_R32G32_SINT: return 8;
        case DXGI_FORMAT_R32_UINT: return 4;
    }
    return 0;
}

DXGI_FORMAT GetTextureFormat(ETextureFormat textureFormat, bool islinear)
{
    // if(textureFormat == ETextureFormat::RGBA4444) return DXGI_FORMAT_B4G4R4A4_UNORM;
    switch(textureFormat)
    {
        case ETextureFormat::RGB24:
        case ETextureFormat::RGBA32:
            return islinear? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case ETextureFormat::RGBFloat:
            return DXGI_FORMAT_R32G32B32_FLOAT;
    }
    
    THROW_ENGINE_EXCEPTION("Unsupported texture format["+std::to_string(textureFormat)+"], now only support RGBA32/RGB32 and RGBAFloat");
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

HRESULT CreateTexture2DView(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContext,  
Texture* pInputTex, ID3D11ShaderResourceView ** ppOutputTextureView)
{
    HRESULT hr = E_FAIL;
    if(pInputTex == nullptr) return hr;
    // auto pDevice = directXGfx.GetDevice();
    // auto pContext = directXGfx.GetContext();
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2D;
    D3D11_TEXTURE2D_DESC ImageTextureDesc = {};
    
    ImageTextureDesc.Width = pInputTex->GetWidth();
    ImageTextureDesc.Height = pInputTex->GetHeight();
    ImageTextureDesc.MipLevels = pInputTex->GetMipMapLevels();
    ImageTextureDesc.ArraySize = 1;
    
    ImageTextureDesc.Format = GetTextureFormat(pInputTex->GetTextureFormat(), pInputTex->IsLinear());
    ImageTextureDesc.SampleDesc.Count = 1;
    ImageTextureDesc.SampleDesc.Quality = 0;
    ImageTextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ImageTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    ImageTextureDesc.MiscFlags = 0;
    

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = ImageTextureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    if(pInputTex->UseMipMap())
    {
        ImageTextureDesc.Usage = D3D11_USAGE_DEFAULT;
        ImageTextureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        ImageTextureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        // we just want to write into first mip level, rest will be generated automatically
        srvDesc.Texture2D.MipLevels = -1;
        // we cannot use subresource data here, because we only have original image data.
        hr = pDevice->CreateTexture2D(&ImageTextureDesc, nullptr, &pTexture2D);
        if(FAILED(hr)) return hr;
        pContext->UpdateSubresource(pTexture2D.Get(), 0, nullptr, pInputTex->GetImageData(), pInputTex->GetPitch(), 0);

        hr = pDevice->CreateShaderResourceView(pTexture2D.Get(), &srvDesc, ppOutputTextureView);
        if(FAILED(hr)) return hr;
        pContext->GenerateMips(*ppOutputTextureView);

    }
    else{
        D3D11_SUBRESOURCE_DATA ImageSubresourceData = {};
        ImageSubresourceData.pSysMem = pInputTex->GetImageData();
        ImageSubresourceData.SysMemPitch = pInputTex->GetPitch();
        hr = pDevice->CreateTexture2D(&ImageTextureDesc, &ImageSubresourceData, &pTexture2D);
        if(FAILED(hr)) return hr;
        
        hr = pDevice->CreateShaderResourceView(pTexture2D.Get(), &srvDesc, ppOutputTextureView);
    }
    return hr;
}

HRESULT CreateSampler(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Texture * pInputTex, ID3D11SamplerState ** ppOutputSampler)
{
    HRESULT hr = E_FAIL;
    if(pInputTex == nullptr) return hr;
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = GetTextureFilterMode(pInputTex->GetFilterMode()); 
    auto wrapmode = GetTextureWrapMode(pInputTex->GetWrapMode());
    samplerDesc.AddressU = wrapmode; 
    samplerDesc.AddressV = wrapmode; 
    samplerDesc.AddressW = wrapmode;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.MaxAnisotropy = pInputTex->GetAnisoLevel() == -1 ? D3D11_REQ_MAXANISOTROPY : pInputTex->GetAnisoLevel();
    hr = pDevice->CreateSamplerState(&samplerDesc, ppOutputSampler);
    return hr;
}