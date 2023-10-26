#include "DirectXGraphics.h"
#include "Exceptions.h"
#include <iostream>
#include "DXUtil.h"
#include "HLSLShader.h"
#include "HLSLShaderHelper.h"
#include "Texture.h"
#include "dxgraphics/BlenderManager.h"
#include "dxgraphics/StencilManager.h"
#include "geometry/Primitive.h"
#include "dxgraph/DirectXRenderGraph.h"

#define LOG(X) std::cout << X << std::endl;
// add ComPtr
namespace wrl = Microsoft::WRL;
HRESULT hr;

/****************************************************************************************/
/*                                      Public Part                                     */
/****************************************************************************************/

DirectXGraphics::DirectXGraphics(HWND &hwnd) : hWnd(hwnd)
{
    LOG("Directx Graphics  constructor begin")
    CreateDevice();
    UpdateRenderSceneViewPort(0, 0, 1280, 720);
    OnResize(1280, 720);
    // m_pShaderHelper = HLSLShaderHelper::GetPtr();
    m_pRenderGraph = std::make_unique<TinyEngine::Graph::DXDefaultRenderGraph>(this);
    LOG("   Directx Graphics  constructor end")
}

DirectXGraphics::~DirectXGraphics()
{
}

void DirectXGraphics::EndFrame()
{
#ifndef NDEBUG
    infoManager.Set();
#endif
    HRESULT hr;
    if (FAILED(hr = pSwap->Present(1u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
        }
        else
        {
            throw GFX_EXCEPT(hr);
        }
    }
}

void DirectXGraphics::ClearBuffer(float red, float green, float blue) noexcept
{
    const float color[] = {red, green, blue, 1.0f};
}

void DirectXGraphics::SetViewport(ViewPort viewPort)
{
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(viewPort.width);
    vp.Height = static_cast<FLOAT>(viewPort.height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = static_cast<FLOAT>(viewPort.pos_x);
    vp.TopLeftY = static_cast<FLOAT>(viewPort.pos_y);
    pContext->RSSetViewports(1u, &vp);
}
void DirectXGraphics::ApplyState(TinyEngine::RenderState *pState)
{
    if (pState == nullptr)
        return;
    // check pState type : TinyEngine::CullMode or TinyEngine::Blend or TinyEngine::DepthStencil
    TinyEngine::RasterState *pRasterState = dynamic_cast<TinyEngine::RasterState *>(pState);
    if (pRasterState != nullptr)
    {
        setRasterizerState(pRasterState->rasterDesc);
        return;
    }

    TinyEngine::BlendState *pBlend = dynamic_cast<TinyEngine::BlendState *>(pState);
    if (pBlend != nullptr)
    {
        ID3D11BlendState *targetBlendState = nullptr;
        GFX_THROW_INFO(TinyEngine::BlenderManager::Get().LoadBlendState(this, pBlend->blendDesc, &targetBlendState));
        pContext->OMSetBlendState(targetBlendState, pBlend->blendDesc.blendFactors, 0xffffffff);
        return;
    }

    TinyEngine::DepthStencilState *pDepthStencil = dynamic_cast<TinyEngine::DepthStencilState *>(pState);
    if (pDepthStencil != nullptr)
    {
        ID3D11DepthStencilState *targetStencilState = nullptr;
        GFX_THROW_INFO(TinyEngine::StencilManager::Get().LoadStencilState(this, pDepthStencil->depthStencilDesc, &targetStencilState));
        pContext->OMSetDepthStencilState(targetStencilState, 0xFF);
        return;
    }
}

void DirectXGraphics::ApplyPass(TinyEngine::ShaderPass &pass)
{
    auto pVertexShader = TinyEngine::EffectManager::Get().VSFindShader<HLSLVertexShader>(pass.vsName);
    auto pPixelShader = TinyEngine::EffectManager::Get().PSFindShader<HLSLPixelShader>(pass.psName);

    if (pVertexShader == nullptr)
        throw std::exception("Vertex Shader not found");
    else
    {
        pContext->VSSetShader(pVertexShader->Get(), nullptr, 0u);
        pVertexShader->SetInputLayout(); // bind input layout
    }

    if (pPixelShader == nullptr)
        pContext->PSSetShader(nullptr, nullptr, 0u);
    else
        pContext->PSSetShader(pPixelShader->Get(), nullptr, 0u);

    ID3D11BlendState *targetBlendState = nullptr;
    GFX_THROW_INFO(TinyEngine::BlenderManager::Get().LoadBlendState(this, pass.blendDesc, &targetBlendState));
    pContext->OMSetBlendState(targetBlendState, pass.blendDesc.blendFactors, 0xffffffff);

    // set stencil state
    ID3D11DepthStencilState *targetStencilState = nullptr;
    GFX_THROW_INFO(TinyEngine::StencilManager::Get().LoadStencilState(this, pass.depthStencilDesc, &targetStencilState));
    pContext->OMSetDepthStencilState(targetStencilState, 0xFF);

    setRasterizerState(pass.rasterDesc);
}
void DirectXGraphics::ApplyPassToRenderList(TinyEngine::ShaderPass &pass, std::vector<Renderer *> &renderers)
{
    ApplyPass(pass);
    auto pVertexShader = TinyEngine::EffectManager::Get().VSFindShader<HLSLVertexShader>(pass.vsName);
    auto pPixelShader = TinyEngine::EffectManager::Get().PSFindShader<HLSLPixelShader>(pass.psName);
    auto &helper = HLSLShaderHelper::Get();
    
    for (size_t k = 0; k < renderers.size(); ++k)
    {
        auto pRenderer = renderers[k];
        if (pRenderer == nullptr) continue;
        
        auto pMat = pRenderer->GetMaterialPtr();
        // update material resource
        if (pVertexShader) pVertexShader->LoadMaterialResource(pMat);
        if (pPixelShader) pPixelShader->LoadMaterialResource(pMat);

        // draw
        pRenderer->UpdateObjBuffer(helper);
        UpdateCBuffer(pObjectConstantBuffer, helper.GetCommonCBufferBySlot(HLSLShaderHelper::PerDrawCBufSlot));
        drawMesh(pVertexShader, pRenderer->GetMesh());
    }
}

void DirectXGraphics::ApplyPassToRenderTarget(TinyEngine::ShaderPass &pass, TinyEngine::RenderTarget *pRenderTarget)
{
    auto pDXRenderTarget = reinterpret_cast<TinyEngine::DirectXRenderTarget *>(pRenderTarget);
    if (pDXRenderTarget == nullptr)
        return;
    else
        BindTexture(0, pDXRenderTarget->pTextureView, Graphics::EBindType::ToPS);

    
    ApplyPass(pass);
    auto pVertexShader = TinyEngine::EffectManager::Get().VSFindShader<HLSLVertexShader>(pass.vsName);

    if (quad == nullptr) quad = Primitive::CreateQuadMeshPtr();
    drawMesh(pVertexShader, *quad);
}

void DirectXGraphics::ApplyPassToMesh(TinyEngine::ShaderPass & pass, Mesh * pMesh)
{
    if(pMesh == nullptr) THROW_ENGINE_EXCEPTION("Mesh should not be nullptr");
    ApplyPass(pass);
    auto pVertexShader = TinyEngine::EffectManager::Get().VSFindShader<HLSLVertexShader>(pass.vsName);
    drawMesh(pVertexShader, *pMesh);
}

std::shared_ptr<TinyEngine::RenderTarget> DirectXGraphics::CreateRenderTarget()
{
    return std::make_shared<TinyEngine::DirectXRenderTarget>(this, m_width, m_height);
}
std::shared_ptr<TinyEngine::DepthStencil> DirectXGraphics::CreateDepthStencil()
{
    return std::make_shared<TinyEngine::DirectXDepthStencil>(this, m_width, m_height);
}
void DirectXGraphics::BindRenderTarget(TinyEngine::RenderTarget *pRenderTarget, TinyEngine::DepthStencil *pDepthStencil)
{
    internalBindRenderTarget(reinterpret_cast<TinyEngine::DirectXRenderTarget *>(pRenderTarget),
                             reinterpret_cast<TinyEngine::DirectXDepthStencil *>(pDepthStencil));
}
void DirectXGraphics::BindDefaultRenderTarget()
{
    pContext->RSSetViewports(1u, &editorViewPort);
    // pContext->OMSetRenderTargets(1u, pPresentRenderTarget->pTargetView.GetAddressOf(), pDepthStencil->Get());
}
void DirectXGraphics::internalBindRenderTarget(TinyEngine::DirectXRenderTarget *pRenderTarget, TinyEngine::DirectXDepthStencil *pDepthStencil)
{
    pContext->RSSetViewports(1u, &fullViewPort);
    if (pRenderTarget == nullptr)
        pContext->OMSetRenderTargets(0u, nullptr, pDepthStencil->Get());
    else if (pDepthStencil == nullptr)
        pContext->OMSetRenderTargets(1u, pRenderTarget->pTargetView.GetAddressOf(), nullptr);
    else
        pContext->OMSetRenderTargets(1u, pRenderTarget->pTargetView.GetAddressOf(), pDepthStencil->Get());
}

/****************************************************************************************/
/*                                      Event  Part                                     */
/****************************************************************************************/
void DirectXGraphics::UpdateRenderSceneViewPort(int pos_x, int pos_y, int width, int height)
{
    ZeroMemory(&editorViewPort, sizeof(editorViewPort));
    editorViewPort.Width = static_cast<FLOAT>(width);
    editorViewPort.Height = static_cast<FLOAT>(height);
    editorViewPort.MinDepth = 0.0f;
    editorViewPort.MaxDepth = 1.0f;
    editorViewPort.TopLeftX = static_cast<FLOAT>(pos_x);
    editorViewPort.TopLeftY = static_cast<FLOAT>(pos_y);
    pContext->RSSetViewports(1u, &editorViewPort);
}

void DirectXGraphics::OnResize(int width, int height)
{
#ifdef DEBUG
    std::cout << "OnResize" << std::endl;
#endif
    if (width == 0 || height == 0)
        return;
    m_width = width;
    m_height = height;

    ZeroMemory(&fullViewPort, sizeof(fullViewPort));
    fullViewPort.Width = static_cast<FLOAT>(width);
    fullViewPort.Height = static_cast<FLOAT>(height);
    fullViewPort.MinDepth = 0.0f;
    fullViewPort.MaxDepth = 1.0f;
    fullViewPort.TopLeftX = 0.0f;
    fullViewPort.TopLeftY = 0.0f;
}

wrl::ComPtr<ID3D11Texture2D> DirectXGraphics::GetBackBuffer()
{
    wrl::ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
    pSwap->ResizeBuffers(0u, m_width, m_height, DXGI_FORMAT_UNKNOWN, 0u);
    GFX_THROW_INFO(pSwap->GetBuffer(0u, __uuidof(ID3D11Texture2D), &pBackBuffer));
    return pBackBuffer;
}
/***********************************************************************************************************/
/*                                            Protected Part                                               */
/***********************************************************************************************************/
void DirectXGraphics::drawMesh(HLSLVertexShader *pVertexShader, Mesh &mesh)
{
    // BindMesh
    // get vertex buffers, strides, offsets from mesh and layout from vertex shader
    // UINT vbcount = pVertexShader->GetInputLayoutDescs().size();
    ID3D11Buffer **pVertexBuffers;
    wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
    UINT *strides;
    UINT *offsets;
    UINT vbcount = pVertexShader->UpdateVertexBuffers(mesh, pVertexBuffers, strides, offsets);

    GFX_THROW_INFO(CreateIndexBuffer(pDevice, mesh.GetIndexBufferAddress(), mesh.GetIndexBufferSize(), mesh.GetIndexStride(), &pIndexBuffer));

    pContext->IASetVertexBuffers(0u, vbcount, pVertexBuffers, strides, offsets);
    // bind index buffer to pipeline
    pContext->IASetIndexBuffer(pIndexBuffer.Get(), GetIndexDataFormat(mesh.GetIndexStride()), 0u);

    // set primitive topology
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // GFX_THROW_INFO_ONLY(pContext->Draw((UINT)std::size(vertices), 0u));
    GFX_THROW_INFO_ONLY(pContext->DrawIndexed(mesh.GetIndexCount(), 0u, 0u));
}

void DirectXGraphics::CreateDevice()
{
    LOG("CreateDevice begin")
    DXGI_SWAP_CHAIN_DESC sd = {};
    UINT creationFlags = 0u;

    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1u;
    sd.BufferDesc.Width = 0u;
    sd.BufferDesc.Height = 0u;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0u;
    sd.BufferDesc.RefreshRate.Denominator = 0u;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1u;
    sd.SampleDesc.Quality = 0u; // (HWND)1234567;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

#ifndef NDEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    GFX_THROW_INFO(
        D3D11CreateDeviceAndSwapChain(
            nullptr,                  // default adapter
            D3D_DRIVER_TYPE_HARDWARE, // use hardware graphics driver
            nullptr,                  // no software device
            creationFlags,            // no flags
            nullptr,                  // default feature level array
            0u,                       // default feature level array size
            D3D11_SDK_VERSION,        // SDK version
            &sd,                      // swap chain description
            &pSwap,                   // swap chain
            &pDevice,                 // device
            nullptr,                  // supported feature level
            &pContext                 // device context
            ));

    LOG("   CreateDevice end")
}
void DirectXGraphics::CreateBuffers(int width, int height)
{
    // !!!!!!!!!!!!!!!!!!!!!
    // pPresentRenderTarget = nullptr; // we must release render target view before resize buffers, otherwise we will get error
    // !!!!!!!!!!!!!!!!!!!!!

    // wrl::ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
    // pSwap->ResizeBuffers(0u, width, height, DXGI_FORMAT_UNKNOWN, 0u);
    // GFX_THROW_INFO(pSwap->GetBuffer(0u, __uuidof(ID3D11Texture2D), &pBackBuffer));

    // // GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRenderTargetView));
    // pPresentRenderTarget = std::make_shared<TinyEngine::DirectXRenderTarget>(this, pBackBuffer.Get());

    // pDepthStencil = std::make_shared<TinyEngine::DirectXDepthStencil>(this, width, height);
}

std::shared_ptr<VertexShader> DirectXGraphics::CreateVertexShader(const std::string &path)
{
    std::shared_ptr<HLSLVertexShader> pShader(new HLSLVertexShader(*this, std::wstring(path.begin(), path.end())));
    return pShader;
}

std::shared_ptr<PixelShader> DirectXGraphics::CreatePixelShader(const std::string &path)
{
    std::shared_ptr<HLSLPixelShader> pShader(new HLSLPixelShader(*this, std::wstring(path.begin(), path.end())));
    return pShader;
}

/****************************************************************************************/
/*                               Render State Operation                                 */
/****************************************************************************************/

void DirectXGraphics::setRasterizerState(const TinyEngine::RasterDesc rasterDesc)
{
    ID3D11RasterizerState *targetRasterizerState = nullptr;
    switch (rasterDesc.cullMode)
    {
    case TinyEngine::ECullMode::Off:
        if (pCullOffRasterizerState == nullptr)
        {
            D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
            rasterizerDesc.CullMode = D3D11_CULL_NONE;
            GFX_THROW_INFO(pDevice->CreateRasterizerState(&rasterizerDesc, &pCullOffRasterizerState));
        }
        targetRasterizerState = pCullOffRasterizerState.Get();
        break;
    case TinyEngine::ECullMode::Front:
        if (pCullBackRasterizerState == nullptr)
        {
            D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
            rasterizerDesc.CullMode = D3D11_CULL_FRONT;
            GFX_THROW_INFO(pDevice->CreateRasterizerState(&rasterizerDesc, &pCullBackRasterizerState));
        }
        targetRasterizerState = pCullBackRasterizerState.Get();
        break;
    case TinyEngine::ECullMode::Back:
        if (pCullFrontRasterizerState == nullptr)
        {
            D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
            rasterizerDesc.CullMode = D3D11_CULL_BACK;
            GFX_THROW_INFO(pDevice->CreateRasterizerState(&rasterizerDesc, &pCullFrontRasterizerState));
        }
        targetRasterizerState = pCullFrontRasterizerState.Get();
        break;
    default:
        break;
    }
    pContext->RSSetState(targetRasterizerState);
}
/****************************************************************************************/
/*                                Constant Buffer Operation                             */
/****************************************************************************************/
void DirectXGraphics::UpdateCBuffer(wrl::ComPtr<ID3D11Buffer> &targetBuf, CBufferData *pCBufferData, Graphics::EBindType bindType)
{
    if (pCBufferData == nullptr)
    {
        LOG("pCBufferData is empty." << __FILE__ << "[" << __LINE__ << "]");
        return;
    }
    int slot = pCBufferData->slot;
    // if(targetBuf == nullptr)
    //     GFX_THROW_INFO(CreateConstantBuffer(pDevice, pCBufferData->pData.get(), pCBufferData->byteWidth, &targetBuf));
    if (pCBufferData->isDirty)
    {
        pCBufferData->isDirty = false;
        // float* data = reinterpret_cast<float*>(pCBufferData->pData.get());
        UpdateCBuffer(targetBuf, pCBufferData->pData.get(), pCBufferData->byteWidth);
    }
    BindCBuffer(slot, targetBuf, bindType);
}

void DirectXGraphics::UpdateCBuffer(wrl::ComPtr<ID3D11Buffer> &targetBuf, BYTE *data, unsigned int bytesize)
{
    if (targetBuf == nullptr)
    {
        GFX_THROW_INFO(CreateConstantBuffer(pDevice, data, bytesize, &targetBuf));
    }
    else
    {
        // update constant buffer by map and unmap
        D3D11_MAPPED_SUBRESOURCE msr;
        pContext->Map(targetBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
        memcpy_s(msr.pData, bytesize, data, bytesize);
        pContext->Unmap(targetBuf.Get(), 0);
        // pContext->UpdateSubresource(targetBuf.Get(), 0, nullptr, bufData.GetAddress(), 0, 0);
    }
}

void DirectXGraphics::BindCBuffer(unsigned int slot, wrl::ComPtr<ID3D11Buffer> &targetBuf, Graphics::EBindType bindType)
{
    if (targetBuf == nullptr)
    {
        // LOG("targetBuf is empty." <<__FILE__ << "[" << __LINE__  << "]");
        return;
    }
    if (bindType == Graphics::EBindType::ToVS)
        pContext->VSSetConstantBuffers(slot, 1u, targetBuf.GetAddressOf());
    else if (bindType == Graphics::EBindType::ToPS)
        pContext->PSSetConstantBuffers(slot, 1u, targetBuf.GetAddressOf());
    else if (bindType == Graphics::EBindType::ToAll)
    {
        pContext->VSSetConstantBuffers(slot, 1u, targetBuf.GetAddressOf());
        pContext->PSSetConstantBuffers(slot, 1u, targetBuf.GetAddressOf());
    }
}

/****************************************************************************************/
/*                                Texture Operation                                     */
/****************************************************************************************/

void DirectXGraphics::BindTexture(unsigned int slot, wrl::ComPtr<ID3D11ShaderResourceView> &pTextureView, Graphics::EBindType bindType)
{
    if (pTextureView == nullptr)
    {
        LOG("targetBuf is empty." << __FILE__ << "[" << __LINE__ << "]");
        return;
    }

    if (bindType == Graphics::EBindType::ToVS)
        pContext->VSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
    else if (bindType == Graphics::EBindType::ToPS)
        pContext->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
    else if (bindType == Graphics::EBindType::ToAll)
    {
        pContext->VSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
        pContext->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
    }
}
void DirectXGraphics::BindSampler(unsigned int slot, wrl::ComPtr<ID3D11SamplerState> &pSamplerState, Graphics::EBindType bindType)
{
    if (pSamplerState == nullptr)
    {
        LOG("pSamplerState is empty." << __FILE__ << "[" << __LINE__ << "]");
        return;
    }

    if (bindType == Graphics::EBindType::ToVS)
        pContext->VSSetSamplers(slot, 1u, pSamplerState.GetAddressOf());
    else if (bindType == Graphics::EBindType::ToPS)
        pContext->PSSetSamplers(slot, 1u, pSamplerState.GetAddressOf());
    else if (bindType == Graphics::EBindType::ToAll)
    {
        pContext->VSSetSamplers(slot, 1u, pSamplerState.GetAddressOf());
        pContext->PSSetSamplers(slot, 1u, pSamplerState.GetAddressOf());
    }
}
