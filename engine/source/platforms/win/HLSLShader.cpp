#include "HLSLShader.h"
#include "Exceptions.h"
#include "DXUtil.h"
#include "HLSLShaderHelper.h"
#include "Mesh.h"
#include "Texture.h"

static HRESULT CreateTexture2DView(DirectXGraphics& directXGfx, Texture* pInputTex, 
    ID3D11ShaderResourceView ** ppOutputTextureView);

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
    HRESULT hr;
#ifndef NDEBUG
    auto & infoManager = gfx.infoManager;
#endif
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflection = nullptr;
    GFX_THROW_INFO(HLSLShaderHelper::LoadShader(gfx, path, entryPoint, shaderModel, pVertexShader, pBlob, pReflection));

    
    GetInputLayoutInfo(pReflection.Get(), &inputLayoutDescs);
    HLSLShaderHelper::LoadShaderInfo(pReflection.Get(), &shaderDescInfo);

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
    samplerDesc.MipLODBias = 0.0f;
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
    HRESULT hr;
    #ifndef NDEBUG
        auto & infoManager = directXGfx.infoManager;
    #endif
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
            auto pTexture = pTexVariable->pTexture.get();
            if(pTexture == nullptr) pTexture = Texture::GetDefaultTexturePtr();
            CreateSampler(pTexture, &pSamplerStateBySlot[samplerInfo.slot]);
            GFX_THROW_INFO(CreateTexture2DView(directXGfx, pTexture, &pTextureViewBySlot[slot]));
        }
        directXGfx.BindTexture(slot, pTextureViewBySlot[slot], Graphics::EBindType::ToVS);
        directXGfx.BindSampler(samplerInfo.slot, pSamplerStateBySlot[samplerInfo.slot], Graphics::EBindType::ToVS);
    }
}

UINT HLSLVertexShader::UpdateVertexBuffers(Mesh& mesh, ID3D11Buffer**& pVertexBuffers,UINT*& strides,UINT*& offsets)
{
    UINT varNum = static_cast<UINT>(inputLayoutDescs.size());
#ifndef NDEBUG
    auto & infoManager = directXGfx.infoManager;
#endif
    HRESULT hr;
    //we need allocate buffer to every variable
    for(UINT i = 0; i<varNum; i++)
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
    HRESULT hr;
#ifndef NDEBUG
    auto & infoManager = gfx.infoManager;
#endif
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflection = nullptr;
    GFX_THROW_INFO(HLSLShaderHelper::LoadShader(gfx, path, entryPoint, shaderModel, pPixelShader, pBlob, pReflection));
    
    // read constant buffer description from shader
    GFX_THROW_INFO(GetShaderInfo(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pReflection));
    // LoadShaderDescInfo(pReflection.Get(), &shaderDescInfo);
    HLSLShaderHelper::LoadShaderInfo(pReflection.Get(), &shaderDescInfo);
    return ;
}


HLSLPixelShader::~HLSLPixelShader()
{
}

ID3D11PixelShader* HLSLPixelShader::Get() const
{
    return pPixelShader.Get();
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
    HRESULT hr;
    #ifndef NDEBUG
        auto & infoManager = directXGfx.infoManager;
    #endif
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
            // CreateTexture2D(pTexture, &pTexture2D);
            CreateSampler(pTexture, &pSamplerStateBySlot[samplerInfo.slot]);
            GFX_THROW_INFO(CreateTexture2DView(directXGfx, pTexture, &pTextureViewBySlot[slot]));
            // directXGfx.pDevice->CreateShaderResourceView(pTexture2D.Get(), nullptr, &pTextureViewBySlot[slot]);
        }
        directXGfx.BindTexture(slot, pTextureViewBySlot[slot], Graphics::EBindType::ToPS);
        directXGfx.BindSampler(samplerInfo.slot, pSamplerStateBySlot[samplerInfo.slot], Graphics::EBindType::ToPS);
    }
}

static HRESULT CreateTexture2DView(DirectXGraphics& directXGfx, Texture* pInputTex, ID3D11ShaderResourceView ** ppOutputTextureView)
{
    HRESULT hr = E_FAIL;
    if(pInputTex == nullptr) return hr;
    auto pDevice = directXGfx.GetDevice();
    auto pContext = directXGfx.GetContext();
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
    srvDesc.Format = GetTextureFormat(pInputTex->GetTextureFormat(), pInputTex->IsLinear());
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