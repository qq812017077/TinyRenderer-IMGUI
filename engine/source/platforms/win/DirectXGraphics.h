#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <memory>
#include <unordered_map>
#ifndef NDEBUG
#include "DxgiInfoManager.h"
#endif
#include "Graphics.h"
#include "EngineWin.h"
#include "effect/Stencil.h"
#include "effect/Pass.h"
#include "dxgraphics/DirectXRenderTarget.h"
#include "dxgraphics/DirectXDepthStencil.h"

namespace wrl = Microsoft::WRL;
class Texture;
struct CBufferData;
class Mesh;
class DirectXGraphics : public Graphics
{
    friend class HLSLVertexShader;
    friend class HLSLPixelShader;
public:
    const static unsigned int MaxCommonSlot = 2u;
    const static unsigned int PerFrameCBufSlot = 0u;
    const static unsigned int PerDrawCBufSlot = 1u;
    const static unsigned int PerLightingCBufSlot = 2u;

    DirectXGraphics(HWND &hwnd);
    DirectXGraphics(const DirectXGraphics&) = delete;
    DirectXGraphics& operator=(const DirectXGraphics&) = delete;
    ~DirectXGraphics();

    void EndFrame() override;
    void ClearBuffer(float red, float green, float blue) noexcept override;
    
    void SetViewport(ViewPort viewPort) override;
    
    void UnbindAllResource() override;

    void ApplyState(TinyEngine::RenderState * pState) override;
    void ApplyPass(TinyEngine::ShaderPass & pass) override;
    void Draw(TinyEngine::RenderEntity* entity, EDrawMode mode = EDrawMode::TriangleList) override;
    void ApplyPassToRenderList(TinyEngine::ShaderPass & pass, std::vector<Renderer*> & renderers) override;
    void ApplyPassToRenderTarget(TinyEngine::ShaderPass & pass, TinyEngine::RenderTarget * pRenderers) override;
    void ApplyPassToMesh(TinyEngine::ShaderPass & pass, Mesh * pMesh, EDrawMode mode = EDrawMode::TriangleList) override;
    
    void setRasterizerState(const TinyEngine::RasterDesc rasterDesc);
    // Constant Buffer Operation
    std::shared_ptr<TinyEngine::RenderTarget> CreateRenderTarget() override;
    std::shared_ptr<TinyEngine::DepthStencil> CreateDepthStencil() override;
    void UpdateCBuffer(wrl::ComPtr<ID3D11Buffer>& targetBuf, CBufferData* pCBufferData, Graphics::EBindType bindType = Graphics::EBindType::ToAll);
    void UpdateCBuffer(wrl::ComPtr<ID3D11Buffer>& targetBuf, BYTE * data, unsigned int bytesize);
    void BindCBuffer(unsigned int slot, wrl::ComPtr<ID3D11Buffer>& targetBuf, EBindType bindType = Graphics::EBindType::ToAll);

    void BindTexture(unsigned int slot, wrl::ComPtr<ID3D11ShaderResourceView>& pTextureView, EBindType bindType = Graphics::EBindType::ToAll);
    void BindSampler(unsigned int slot, wrl::ComPtr<ID3D11SamplerState>& pTextureView, EBindType bindType = Graphics::EBindType::ToAll);
    void BindRenderTarget(TinyEngine::RenderTarget* pRenderTarget, TinyEngine::DepthStencil * pDepthStencil) override;
    void BindDefaultRenderTarget() override;
    ID3D11Device* GetDevice() const noexcept { return pDevice.Get(); }
    ID3D11DeviceContext* GetContext() const noexcept { return pContext.Get(); }
    IDXGISwapChain * GetSwap() const noexcept { return pSwap.Get(); }
    //Events
    void UpdateRenderSceneViewPort(float pos_x, float pos_y, float width, float height) override;
    void OnResize(int width, int height) override;
    size_t PickGuidOfGameObject(float u, float v) override ;
    wrl::ComPtr<ID3D11Texture2D> GetBackBuffer();
    D3D11_VIEWPORT GetEditorViewPort() const noexcept { return editorViewPort; }
    D3D11_VIEWPORT GetFullViewPort() const noexcept { return fullViewPort; }
    #ifndef NDEBUG
    DxgiInfoManager& GetInfoManager() { return infoManager; }
    #endif

    wrl::ComPtr<ID3D11Buffer> & GetObjectConstantBuffer() {return pObjectConstantBuffer;}
protected:
    void internalBindRenderTarget(TinyEngine::DirectXRenderTarget* pRenderTarget, TinyEngine::DirectXDepthStencil * pDepthStencil);
    void drawMesh(HLSLVertexShader* pVertexShader, Mesh & mesh, EDrawMode mode = EDrawMode::TriangleList);
    void CreateDevice() ;
    void CreateBuffers(int width, int height);

    std::shared_ptr<VertexShader> CreateVertexShader(const std::string& path) override;
    std::shared_ptr<PixelShader> CreatePixelShader(const std::string& path) override;
    
    
private:


    wrl::ComPtr<ID3D11Device> pDevice= nullptr;
    wrl::ComPtr<ID3D11DeviceContext> pContext = nullptr;
    wrl::ComPtr<IDXGISwapChain> pSwap = nullptr;

    wrl::ComPtr<ID3D11Buffer> pObjectConstantBuffer = nullptr;
    //Render State
    wrl::ComPtr<ID3D11RasterizerState> pCullOffRasterizerState = nullptr;
    wrl::ComPtr<ID3D11RasterizerState> pCullBackRasterizerState = nullptr;
    wrl::ComPtr<ID3D11RasterizerState> pCullFrontRasterizerState = nullptr;
    
    HLSLVertexShader * pCurrentVertexShader = nullptr;
    HLSLPixelShader * pCurrentPixelShader = nullptr;


    D3D11_VIEWPORT editorViewPort;
    D3D11_VIEWPORT fullViewPort;
#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif
    HWND hWnd;
    bool bindedImgui = false;
};
