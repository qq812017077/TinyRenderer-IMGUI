#include "HLSLShader.h"
#include "Exceptions.h"
#include "DXUtil.h"
#include "Mesh.h"
#include "Texture.h"

static HRESULT CreateTexture(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Texture* pTex, ID3D11Texture2D ** ppOutputTexture,
    Texture::EType textureType);

HLSLVertexShader::HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path):
    HLSLVertexShader(gfx, path, "main", "vs_5_0")
{
}

HLSLVertexShader::HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint):
    HLSLVertexShader(gfx, path, entryPoint, "vs_5_0")
{
}

HLSLVertexShader::HLSLVertexShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint, LPCSTR shaderModel):
    directXGfx(gfx)
{

    // if has postfix, remove it
    std::wstring shaderName = path;
    if (shaderName.find(L".hlsl") != std::string::npos)
    {
        shaderName = shaderName.substr(0, shaderName.find(L".hlsl"));
    }
    // add '.cso' postfix
    auto output = shaderName.append(L".cso").c_str();
    HRESULT hr;
#ifndef NDEBUG
    auto & infoManager = gfx.infoManager;
#endif
    // load vertex shader
    GFX_THROW_INFO(CreateShaderFromFile(output, path.c_str(), entryPoint, shaderModel, &pBlob));
    GFX_THROW_INFO(gfx.pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

    // read constant buffer description from shader
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflection = nullptr;
    D3D11_SHADER_DESC shaderDesc;
    GFX_THROW_INFO(GetShaderInfo(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pReflection, &shaderDesc));
    GetInputLayoutInfo(pReflection.Get(), shaderDesc, &inputLayoutDescs);
    LoadShaderDescInfo(pReflection.Get(), shaderDesc, &shaderDescInfo);
    return ;
}


HLSLVertexShader::~HLSLVertexShader()
{
    for(int i = 0; i < inputLayoutDescs.size(); i++)
    {
        delete inputLayoutDescs[i].SemanticName;
    }
}

ID3D11VertexShader* HLSLVertexShader::Get() const
{
    return pVertexShader.Get();
}

bool HLSLVertexShader::HasConstantBuffer() const
{
    return pMaterialConstantBuffer != nullptr;
}

wrl::ComPtr<ID3D11Buffer> HLSLVertexShader::GetConstantBuffer() const
{
    return pMaterialConstantBuffer;
}

bool HLSLVertexShader::CreateTexture2D(Texture* pInputTex, ID3D11Texture2D ** ppOutputTexture2D)
{
    if(pInputTex == nullptr) return false;
    HRESULT hr;
#ifndef NDEBUG
    auto & infoManager = directXGfx.infoManager;
#endif
    GFX_THROW_INFO(CreateTexture(directXGfx.pDevice, pInputTex, ppOutputTexture2D, Texture::EType::TEXTURE2D));
    return true;
}

bool HLSLVertexShader::CreateSampler(Texture * pInputTex, ID3D11SamplerState ** ppOutputSampler)
{
    if(pInputTex == nullptr) return false;
    HRESULT hr;
#ifndef NDEBUG
    auto & infoManager = directXGfx.infoManager;
#endif
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = GetTextureFilterMode(pInputTex->GetFilterMode()); 
    auto wrapmode = GetTextureWrapMode(pInputTex->GetWrapMode());
    samplerDesc.AddressU = wrapmode; 
    samplerDesc.AddressV = wrapmode; 
    samplerDesc.AddressW = wrapmode; 
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    GFX_THROW_INFO(directXGfx.pDevice->CreateSamplerState(&samplerDesc, ppOutputSampler));
    return true;
}

void HLSLVertexShader::SetInputLayout(Mesh & mesh)
{
    SetInputElementDescription(mesh);
    HRESULT hr;
    auto & infoManager = directXGfx.infoManager;
    GFX_THROW_INFO(directXGfx.pDevice->CreateInputLayout(
            inputLayoutDescs.data(),
            static_cast<UINT>(inputLayoutDescs.size()),
            pBlob->GetBufferPointer(), 
            pBlob->GetBufferSize(),
            &pInputLayout));

    // bind input layout to pipeline
    directXGfx.pContext->IASetInputLayout(pInputLayout.Get());
}


void HLSLVertexShader::SetInputElementDescription(Mesh & mesh)
{
    //we need set AlignedByteOffset and Format for each element based on mesh's vertex type
    for(int i = 0, imax = static_cast<int>(inputLayoutDescs.size()); i < imax; i++)
    {
        inputLayoutDescs[i].Format = GetVertexDataFormat(Mesh::GetVertexDataType(inputLayoutDescs[i].SemanticName));
        inputLayoutDescs[i].AlignedByteOffset = Mesh::GetAlignedByteOffset(inputLayoutDescs[i].SemanticName, inputLayoutDescs[i].SemanticIndex);
    }
}


// *********************************************************************************************************************
// HLSLPixelShader implementation
// *********************************************************************************************************************

HLSLPixelShader::HLSLPixelShader(DirectXGraphics& gfx, const std::wstring& path):
    HLSLPixelShader(gfx, path, "main", "ps_5_0")
{
}

HLSLPixelShader::HLSLPixelShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint):
    HLSLPixelShader(gfx, path, entryPoint, "ps_5_0")
{
}
/**
 * @brief Construct a new HLSLPixelShader::HLSLPixelShader object
 * 
 * @param gfx 
 * @param path 
 */
HLSLPixelShader::HLSLPixelShader(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint, LPCSTR shaderModel):
    directXGfx(gfx)
{
    // if has postfix, remove it
    std::wstring shaderName = path;
    if (shaderName.find(L".hlsl") != std::string::npos)
    {
        shaderName = shaderName.substr(0, shaderName.find(L".hlsl"));
    }
    auto output = shaderName.append(L".cso").c_str();
    HRESULT hr;

    #ifndef NDEBUG
    auto & infoManager = directXGfx.infoManager;
#endif
    // load pixel shader
    //convert string to WCHAR *
    wrl::ComPtr<ID3DBlob> pBlob;
    GFX_THROW_INFO(CreateShaderFromFile(output, path.c_str(), entryPoint, shaderModel, &pBlob));
    GFX_THROW_INFO(directXGfx.pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
    
    // read constant buffer description from shader
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflection = nullptr;
    D3D11_SHADER_DESC shaderDesc;
    GFX_THROW_INFO(GetShaderInfo(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pReflection, &shaderDesc));
    LoadShaderDescInfo(pReflection.Get(), shaderDesc, &shaderDescInfo);
    return ;
}


HLSLPixelShader::~HLSLPixelShader()
{
}


ID3D11PixelShader* HLSLPixelShader::Get() const
{
    return pPixelShader.Get();
}

bool HLSLPixelShader::HasConstantBuffer() const
{
    return pMaterialConstantBuffer != nullptr;
}

wrl::ComPtr<ID3D11Buffer> HLSLPixelShader::GetConstantBuffer() const
{
    return pMaterialConstantBuffer;
}

bool HLSLPixelShader::CreateTexture2D(Texture* pInputTex, ID3D11Texture2D ** ppOutputTexture2D)
{
    if(pInputTex == nullptr) return false;
    HRESULT hr;
#ifndef NDEBUG
    auto & infoManager = directXGfx.infoManager;
#endif
    GFX_THROW_INFO(CreateTexture(directXGfx.pDevice, pInputTex, ppOutputTexture2D, Texture::EType::TEXTURE2D));
    return true;
}


bool HLSLPixelShader::CreateSampler(Texture * pInputTex, ID3D11SamplerState ** ppOutputSampler)
{
    if(pInputTex == nullptr) return false;
    HRESULT hr;
#ifndef NDEBUG
    auto & infoManager = directXGfx.infoManager;
#endif
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = GetTextureFilterMode(pInputTex->GetFilterMode()); 
    auto wrapmode = GetTextureWrapMode(pInputTex->GetWrapMode());
    samplerDesc.AddressU = wrapmode; 
    samplerDesc.AddressV = wrapmode; 
    samplerDesc.AddressW = wrapmode; 
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    GFX_THROW_INFO(directXGfx.pDevice->CreateSamplerState(&samplerDesc, ppOutputSampler));
    return true;
}
static HRESULT CreateTexture(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, Texture* pTex, ID3D11Texture2D ** ppOutputTexture,
    Texture::EType textureType)
{
    HRESULT hr = E_FAIL;
    if(textureType == Texture::EType::TEXTURE2D)
    {
        D3D11_TEXTURE2D_DESC ImageTextureDesc = {};
    
        ImageTextureDesc.Width = pTex->GetWidth();
        ImageTextureDesc.Height = pTex->GetHeight();
        ImageTextureDesc.MipLevels = pTex->GetMipMapLevels();
        ImageTextureDesc.ArraySize = 1;
        
        ImageTextureDesc.Format = GetTextureFormat(pTex->GetTextureFormat(), pTex->IsLinear());
        ImageTextureDesc.SampleDesc.Count = 1;
        ImageTextureDesc.SampleDesc.Quality = 0;
        ImageTextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
        ImageTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA ImageSubresourceData = {};
        ImageSubresourceData.pSysMem = pTex->GetImageData();
        ImageSubresourceData.SysMemPitch = pTex->GetPitch();
        
        hr = pDevice->CreateTexture2D(&ImageTextureDesc,
                                        &ImageSubresourceData,
                                        ppOutputTexture
                                        );
        if (FAILED(hr)) return hr;
    }else
    {
        throw std::exception("Not implemented");
    }
    return hr;
}