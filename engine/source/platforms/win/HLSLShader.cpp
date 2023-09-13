#include "HLSLShader.h"
#include "Exceptions.h"
#include "DXUtil.h"
#include "HLSLShaderHelper.h"
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
    GFX_THROW_INFO(GetShaderInfo(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pReflection));
    GetInputLayoutInfo(pReflection.Get(), &inputLayoutDescs);
    HLSLShaderHelper::LoadShaderInfo(reinterpret_cast<const char *>(shaderName.c_str()), pReflection.Get(), &shaderDescInfo);
    // LoadShaderDescInfo(pReflection.Get(), &shaderDescInfo);

    for(int i = 0; i < inputLayoutDescs.size(); i++)
    {
        pVertexBuffers.emplace_back(nullptr);
        vertexBufferPtrs.emplace_back(nullptr);
        UINT stride = GetDataStride(inputLayoutDescs[i].Format);
        strides.push_back(stride);
        offsets.push_back(0);
    }
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

void HLSLVertexShader::SetInputLayout()
{
    if(pInputLayout == nullptr)
    {
        HRESULT hr;
        auto & infoManager = directXGfx.infoManager;
        GFX_THROW_INFO(directXGfx.pDevice->CreateInputLayout(
                inputLayoutDescs.data(),
                static_cast<UINT>(inputLayoutDescs.size()),
                pBlob->GetBufferPointer(),
                pBlob->GetBufferSize(),
                &pInputLayout)); 
    }
    // bind input layout to pipeline
    directXGfx.pContext->IASetInputLayout(pInputLayout.Get());
}

void HLSLVertexShader::UpdateConstantBuffer()
{
    for(auto & pair: shaderDescInfo.GetCBuffers())
    {
        unsigned int slot = pair.first;
        if(slot <= DirectXGraphics::MaxCommonSlot) continue;
        directXGfx.UpdateCBuffer(pConstantBuffersBySlot[slot], pair.second.get(),Graphics::EBindType::ToVS);
    }
}

void HLSLVertexShader::UpdateTexture()
{
    for(auto & pair: shaderDescInfo.GetTextures())
    {
        auto & name = pair.first;
        TextureVariable * pTexVariable = pair.second.get();
        unsigned int slot = pTexVariable->slot;
        SamplerInfo samplerInfo = GetSamplerInfoByName(pTexVariable->samplerName.c_str(), (int)slot);
        if(pTexVariable->isDirty)
        {
            pTexVariable->isDirty = false;
            //Update
            Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2D;
            auto pTexture = pTexVariable->pTexture.get();
            if(pTexture == nullptr) pTexture = Texture::GetDefaultTexturePtr();
            CreateTexture2D(pTexture, &pTexture2D);
            CreateSampler(pTexture, &pSamplerStateBySlot[samplerInfo.slot]);
            directXGfx.pDevice->CreateShaderResourceView(pTexture2D.Get(), nullptr, &pTextureViewBySlot[slot]);
        }
        directXGfx.BindTexture(slot, pTextureViewBySlot[slot], Graphics::EBindType::ToVS);
        directXGfx.BindSampler(samplerInfo.slot, pSamplerStateBySlot[samplerInfo.slot], Graphics::EBindType::ToVS);
    }
}

UINT HLSLVertexShader::UpdateVertexBuffers(Mesh& mesh, ID3D11Buffer**& pVertexBuffers,UINT*& strides,UINT*& offsets)
{
    UINT varNum = inputLayoutDescs.size();
#ifndef NDEBUG
    auto & infoManager = directXGfx.infoManager;
#endif
    HRESULT hr;
    //we need allocate buffer to every variable
    for(int i = 0; i<varNum; i++)
    {
        UINT bufferSize;
        auto bufferAddress = mesh.GetVertexBuffer(inputLayoutDescs[i].SemanticName, bufferSize);
        if(bufferSize != 0)
        {
            GFX_THROW_INFO(CreateVertexBuffer(directXGfx.pDevice, bufferAddress, bufferSize, this->strides[i], &this->pVertexBuffers[i]));
            // if(this->pVertexBuffers[i] == nullptr)
            // {
            //     GFX_THROW_INFO(CreateVertexBuffer(directXGfx.pDevice, bufferAddress, bufferSize, this->strides[i], &this->pVertexBuffers[i]));
            // }else
            // {

            // }
            vertexBufferPtrs[i] = this->pVertexBuffers[i].Get();
        }
    }

    pVertexBuffers = vertexBufferPtrs.data();
    strides = this->strides.data();
    offsets = this->offsets.data();
    return varNum;
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
    GFX_THROW_INFO(GetShaderInfo(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pReflection));
    // LoadShaderDescInfo(pReflection.Get(), &shaderDescInfo);
    HLSLShaderHelper::LoadShaderInfo(reinterpret_cast<const char *>(shaderName.c_str()), pReflection.Get(), &shaderDescInfo);
    return ;
}


HLSLPixelShader::~HLSLPixelShader()
{
}

ID3D11PixelShader* HLSLPixelShader::Get() const
{
    return pPixelShader.Get();
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


void HLSLPixelShader::UpdateConstantBuffer()
{
    for(auto & pair: shaderDescInfo.GetCBuffers())
    {
        unsigned int slot = pair.first;
        if(slot <= DirectXGraphics::MaxCommonSlot) continue;
        directXGfx.UpdateCBuffer(pConstantBuffersBySlot[slot], pair.second.get(), Graphics::EBindType::ToPS);
    }
}

void HLSLPixelShader::UpdateTexture()
{
    for(auto & pair: shaderDescInfo.GetTextures())
    {
        auto & name = pair.first;
        TextureVariable * pTexVariable = pair.second.get();
        unsigned int slot = pTexVariable->slot;
        SamplerInfo samplerInfo = GetSamplerInfoByName(pTexVariable->samplerName.c_str(), (int)slot);
        if(pTexVariable->isDirty)
        {
            pTexVariable->isDirty = false;
            //Update
            Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2D;
            auto pTexture = pTexVariable->pTexture.get();
            if(pTexture == nullptr) pTexture = Texture::GetDefaultTexturePtr();
            CreateTexture2D(pTexture, &pTexture2D);
            CreateSampler(pTexture, &pSamplerStateBySlot[samplerInfo.slot]);
            directXGfx.pDevice->CreateShaderResourceView(pTexture2D.Get(), nullptr, &pTextureViewBySlot[slot]);
        }
        directXGfx.BindTexture(slot, pTextureViewBySlot[slot], Graphics::EBindType::ToPS);
        directXGfx.BindSampler(samplerInfo.slot, pSamplerStateBySlot[samplerInfo.slot], Graphics::EBindType::ToPS);
    }
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