#include "HLSLShader.h"
#include "Exceptions.h"
#include "DXUtil.h"
#include "Mesh.h"

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
    GFX_THROW_INFO(GetShaderInfo(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),&uniformVars, &inputLayoutDescs));
}

//HLSLVertexShader::HLSLVertexShader(HLSLVertexShader&& other) noexcept:
//    directXGfx(other.directXGfx),
//    pVertexShader(std::move(other.pVertexShader)),
//    pBlob(std::move(other.pBlob)),
//    pMaterialConstantBuffer(std::move(other.pMaterialConstantBuffer)),
//    pInputLayout(std::move(other.pInputLayout)),
//    inputLayoutDescs(std::move(other.inputLayoutDescs))
//{
//    other.pVertexShader = nullptr;
//    other.pBlob = nullptr;
//    other.pMaterialConstantBuffer = nullptr;
//    other.pInputLayout = nullptr;
//}

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

void HLSLVertexShader::updateMaterialUniformBuffer(UniformBuffer uniformBuffer)
{
    if(uniformBuffer.bytesize() == 0) return ;
    directXGfx.UpdateCBuffer(pMaterialConstantBuffer, uniformBuffer);
// #ifndef NDEBUG
//     auto& infoManager = directXGfx.infoManager;
// #endif
//     if(pMaterialConstantBuffer == nullptr)
//     {
//         // bind constant buffer to pixel shader
//         HRESULT hr;
//         GFX_THROW_INFO(CreateConstantBuffer(directXGfx.pDevice, uniformBuffer.GetAddress(), uniformBuffer.bytesize(), &pMaterialConstantBuffer));
//     }
//     else
//         directXGfx.pContext->UpdateSubresource(pMaterialConstantBuffer.Get(), 0u, nullptr, uniformBuffer.GetAddress(), 0u, 0u);
}

void HLSLVertexShader::bindToPipeline()
{
    directXGfx.pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
    if(pMaterialConstantBuffer != nullptr)
    {
        directXGfx.pContext->VSSetConstantBuffers(DirectXGraphics::MaterialCBufSlot, 1u, pMaterialConstantBuffer.GetAddressOf());
    }
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

// std::shared_ptr<HLSLVertexShader> HLSLVertexShader::Load(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint, LPCSTR shaderModel)
// {   
//     std::shared_ptr<HLSLVertexShader> pShader(new HLSLVertexShader(gfx, path, entryPoint, shaderModel));
//     return pShader;
// }

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
    GFX_THROW_INFO(GetShaderInfo(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),&uniformVars, nullptr));
}

//HLSLPixelShader::HLSLPixelShader(HLSLPixelShader&& other) noexcept:
//    directXGfx(other.directXGfx),
//    pPixelShader(std::move(other.pPixelShader)),
//    pBlob(std::move(other.pBlob)),
//    pMaterialConstantBuffer(std::move(other.pMaterialConstantBuffer))
//{
//    other.pPixelShader = nullptr;
//    other.pBlob = nullptr;
//    other.pMaterialConstantBuffer = nullptr;
//}

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

void HLSLPixelShader::updateMaterialUniformBuffer(UniformBuffer uniformBuffer)
{
    if(uniformBuffer.bytesize() == 0) return ;
    directXGfx.UpdateCBuffer(pMaterialConstantBuffer, uniformBuffer);
// #ifndef NDEBUG
//     auto& infoManager = directXGfx.infoManager;
// #endif
//     if(pMaterialConstantBuffer == nullptr)
//     {
//         // bind constant buffer to pixel shader
//         HRESULT hr;
//         GFX_THROW_INFO(CreateConstantBuffer(directXGfx.pDevice, uniformBuffer.GetAddress(), uniformBuffer.bytesize(), &pMaterialConstantBuffer));
//     }
//     else
//         directXGfx.pContext->UpdateSubresource(pMaterialConstantBuffer.Get(), 0u, nullptr, uniformBuffer.GetAddress(), 0u, 0u);
}

void HLSLPixelShader::bindToPipeline()
{
    directXGfx.pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
    if(pMaterialConstantBuffer != nullptr)
    {
        directXGfx.pContext->PSSetConstantBuffers(DirectXGraphics::MaterialCBufSlot, 1u, pMaterialConstantBuffer.GetAddressOf());
    }
}


// std::shared_ptr<HLSLPixelShader> HLSLPixelShader::Load(DirectXGraphics& gfx, const std::wstring& path, LPCSTR entryPoint, LPCSTR shaderModel)
// {
//     std::shared_ptr<HLSLPixelShader> pShader(new HLSLPixelShader(gfx, path, entryPoint, shaderModel));
//     return pShader;
// }



