#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#ifndef NDEBUG
#include "DxgiInfoManager.h"
#endif
#include <wrl.h>
#include "Graphics.h"
#include "EngineWin.h"
#include <memory>
#include <unordered_map>
#include "effect/Stencil.h"
#include "pass/DirectXShadingEffectPass.h"

namespace wrl = Microsoft::WRL;
class Texture;
struct CBufferData;
class Renderer;
class DirectXGraphics : public Graphics
{
    friend class HLSLVertexShader;
    friend class HLSLPixelShader;
    friend class TinyEngine::DirectXShadingEffectPass;
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
    
    void DrawTestTriangle(float angle=0.0f) override;
    void DrawAll(TinyEngine::FrameBuffer * pFrameBuffer) override;
    void Apply(TinyEngine::ShaderPass & pass, std::vector<Renderer*> & renderers) override;
    
    ID3D11Device* GetDevice() const noexcept { return pDevice.Get(); }
    ID3D11DeviceContext* GetContext() const noexcept { return pContext.Get(); }
    //Events
    virtual void UpdateRenderSceneViewPort(int pos_x, int pos_y, int width, int height) override;
    virtual void OnResize(int width, int height) override;

    #ifndef NDEBUG
    DxgiInfoManager& GetInfoManager() { return infoManager; }
    #endif
protected:
    void drawMesh(HLSLVertexShader* pVertexShader, Renderer* pRenderer);
    void CreateDevice() ;
    void CreateBuffers(int width, int height);

    std::shared_ptr<VertexShader> CreateVertexShader(const std::string& path) override;
    std::shared_ptr<PixelShader> CreatePixelShader(const std::string& path) override;
    
    // void LoadMaterial(Material * pMat, ShaderBase * pShader);
    void setRasterizerState(const TinyEngine::ECullMode cullMode);
    // Constant Buffer Operation
    void UpdateCBuffer(wrl::ComPtr<ID3D11Buffer>& targetBuf, CBufferData* pCBufferData, Graphics::EBindType bindType = Graphics::EBindType::ToAll);
    void UpdateCBuffer(wrl::ComPtr<ID3D11Buffer>& targetBuf, BYTE * data, unsigned int bytesize);
    void BindCBuffer(unsigned int slot, wrl::ComPtr<ID3D11Buffer>& targetBuf, EBindType bindType = Graphics::EBindType::ToAll);

    void BindTexture(unsigned int slot, wrl::ComPtr<ID3D11ShaderResourceView>& pTextureView, EBindType bindType = Graphics::EBindType::ToAll);
    void BindSampler(unsigned int slot, wrl::ComPtr<ID3D11SamplerState>& pTextureView, EBindType bindType = Graphics::EBindType::ToAll);
private:
    wrl::ComPtr<ID3D11Device> pDevice= nullptr;
    wrl::ComPtr<ID3D11DeviceContext> pContext = nullptr;
    wrl::ComPtr<IDXGISwapChain> pSwap = nullptr;
    wrl::ComPtr<ID3D11RenderTargetView> pRenderTargetView = nullptr;
    wrl::ComPtr<ID3D11DepthStencilView> pDepthStencilView = nullptr;

    wrl::ComPtr<ID3D11Buffer> pObjectConstantBuffer = nullptr;
    wrl::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;

    wrl::ComPtr<ID3D11BlendState> pDefaultTransparentBlendState = nullptr;
    wrl::ComPtr<ID3D11BlendState> pDefaultOpaqueBlendState = nullptr;
    //Render State
    wrl::ComPtr<ID3D11RasterizerState> pCullOffRasterizerState = nullptr;
    wrl::ComPtr<ID3D11RasterizerState> pCullBackRasterizerState = nullptr;
    wrl::ComPtr<ID3D11RasterizerState> pCullFrontRasterizerState = nullptr;

    std::shared_ptr<Material> pMaskMat;
    std::shared_ptr<Material> pOutlineMat;
    //render pass
    TinyEngine::DirectXShadingEffectPass m_shading_pass;
#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif
    HWND hWnd;
    bool bindedImgui = false;
};
