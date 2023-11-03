#include "HLSLShader.h"
#include "Exceptions.h"
#include "DXUtil.h"
#include "HLSLShaderHelper.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

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

    for(unsigned int i = 0; i < inputLayoutDescs.size(); i++)
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
    for(unsigned int i = 0; i < inputLayoutDescs.size(); i++)
    {
        delete inputLayoutDescs[i].SemanticName;
    }
}

ID3D11VertexShader* HLSLVertexShader::Get() const
{
    return pVertexShader.Get();
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

void HLSLVertexShader::LoadMaterialResource(Material * pMat)
{
    auto shaderResource = pMat->GetUpdatedShaderResourcePtr(this);

    for(auto & pair: shaderResource->CBufferBySlot)
    {
        unsigned int slot = pair.first;
        if(slot <= DirectXGraphics::MaxCommonSlot) continue;
        directXGfx.UpdateCBuffer(pConstantBuffersByID[pair.second->uniqueID], pair.second.get(),Graphics::EBindType::ToVS);
    }
    
    HRESULT hr;
    #ifndef NDEBUG
        auto & infoManager = directXGfx.infoManager;
    #endif
    for(auto & pair: shaderResource->textures)
    {
        auto & name = pair.first;
        TextureVariable * pTexVariable = pair.second.get();
        size_t texId = pTexVariable->uniqueID;
        int slot = pTexVariable->slot();
        SamplerInfo& samplerInfo = shaderResource->GetSamplerInfoByName(pTexVariable->samplerName.c_str(), slot);
        size_t samplerId = samplerInfo.uniqueID;
        if(pTexVariable->isDirty)
        {
            pTexVariable->isDirty = false;
            //Update
            auto pTexture = pTexVariable->pTexture.get();
            if(pTexture == nullptr) pTexture = Texture::GetDefaultTexturePtr();
            GFX_THROW_INFO(CreateSampler(directXGfx.pDevice, pTexture, &pSamplerStateByID[samplerId]));
            GFX_THROW_INFO(CreateTexture2DView(directXGfx.pDevice, directXGfx.pContext, pTexture, &pTextureViewByID[texId]));
        }
        directXGfx.BindTexture(slot, pTextureViewByID[texId], Graphics::EBindType::ToVS);
        directXGfx.BindSampler(samplerInfo.bindPoint, pSamplerStateByID[samplerId], Graphics::EBindType::ToVS);
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



void HLSLPixelShader::LoadMaterialResource(Material * pMat)
{
    auto shaderResource = pMat->GetUpdatedShaderResourcePtr(this);

    for(auto & pair: shaderResource->CBufferBySlot)
    {
        unsigned int slot = pair.first;
        if(slot <= DirectXGraphics::MaxCommonSlot) continue;
        directXGfx.UpdateCBuffer(pConstantBuffersByID[pair.second->uniqueID], pair.second.get(),Graphics::EBindType::ToPS);
    }
    
    HRESULT hr;
    #ifndef NDEBUG
        auto & infoManager = directXGfx.infoManager;
    #endif
    for(auto & pair: shaderResource->textures)
    {
        auto & name = pair.first;
        TextureVariable * pTexVariable = pair.second.get();
        size_t texId = pTexVariable->uniqueID;
        int slot = pTexVariable->slot();
        if(HLSLShaderHelper::IsGlobalTextureSlot(slot)) continue; // skip global texture
        SamplerInfo& samplerInfo = shaderResource->GetSamplerInfoByName(pTexVariable->samplerName.c_str(), (int)slot);
        size_t samplerId = samplerInfo.uniqueID;
        if(pTexVariable->isDirty)
        {
            pTexVariable->isDirty = false;
            //Update
            auto pTexture = pTexVariable->pTexture.get();
            if(pTexture == nullptr) pTexture = Texture::GetDefaultTexturePtr();
            GFX_THROW_INFO(CreateSampler(directXGfx.pDevice, pTexture, &pSamplerStateByID[samplerId]));
            GFX_THROW_INFO(CreateTexture2DView(directXGfx.pDevice, directXGfx.pContext, pTexture, &pTextureViewByID[texId]));
        }
        directXGfx.BindTexture(slot, pTextureViewByID[texId], Graphics::EBindType::ToPS);
        directXGfx.BindSampler(samplerInfo.bindPoint, pSamplerStateByID[samplerId], Graphics::EBindType::ToPS);
    }
}
