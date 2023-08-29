#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")
#ifndef NDEBUG
#include "DxgiInfoManager.h"
#endif
#include <wrl.h>
#include "Graphics.h"
#include "EngineWin.h"
#include <memory>

namespace wrl = Microsoft::WRL;
class Texture;
struct CBufferData;
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

    void BindImgui() override;
    void EndFrame() override;
    void ClearBuffer(float red, float green, float blue) noexcept override;
    
    void DrawTestTriangle(float angle=0.0f) override;
    void DrawAll() override;

    ID3D11Device* GetDevice() const noexcept { return pDevice.Get(); }
    ID3D11DeviceContext* GetContext() const noexcept { return pContext.Get(); }
    //Events
    void OnResize(int width, int height) override;
protected:
    void CreateDevice() override;
    void CreateRenderTarget() override;
    void ClearupDevice() override;
    void ClearupRenderTarget() override;

    std::shared_ptr<VertexShader> CreateVertexShader(const std::string& path) override;
    std::shared_ptr<PixelShader> CreatePixelShader(const std::string& path) override;
    
    // Material Operation
    void LoadMaterial(Material & material) override;

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
    wrl::ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
    wrl::ComPtr<ID3D11DepthStencilView> pDepthStencilView = nullptr;
    wrl::ComPtr<ID3D11Buffer> pFrameConstantBuffer = nullptr;
    wrl::ComPtr<ID3D11Buffer> pLightingConstantBuffer = nullptr;
    wrl::ComPtr<ID3D11Buffer> pObjectConstantBuffer = nullptr;
    wrl::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;
    wrl::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
    wrl::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
    //Render State
    wrl::ComPtr<ID3D11RasterizerState> pRasterizerState = nullptr;

#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif
    HWND hWnd;
    bool bindedImgui = false;
};
