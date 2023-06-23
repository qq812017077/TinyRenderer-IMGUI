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
#include "UniformBuffer.h"
namespace wrl = Microsoft::WRL;

class DirectXGraphics : public Graphics
{
friend class HLSLVertexShader;
friend class HLSLPixelShader;
public:
    const static unsigned int FrameCBufSlot = 0u;
    const static unsigned int MaterialCBufSlot = 1u;
    const static unsigned int ObjectCBufSlot = 2u;

    DirectXGraphics(HWND &hwnd);
    DirectXGraphics(const DirectXGraphics&) = delete;
    DirectXGraphics& operator=(const DirectXGraphics&) = delete;
    ~DirectXGraphics();
    void EndFrame() override;
    void ClearBuffer(float red, float green, float blue) noexcept override;
    void DrawTestTriangle(float angle=0.0f) override;
    void DrawAll() override;

    //Events
    void OnResize(int width, int height) override;
protected:
    void CreateDevice() override;
    void CreateRenderTarget() override;
    void ClearupDevice() override;
    void ClearupRenderTarget() override;

    std::shared_ptr<VertexShader> CreateVertexShader(const std::string& path) override;
    std::shared_ptr<PixelShader> CreatePixelShader(const std::string& path) override;
    
    // Constant Buffer Operation
    void UpdateCBuffer(wrl::ComPtr<ID3D11Buffer>& targetBuf, UniformBuffer& bufData);
    void BindCBuffer(unsigned int slot, wrl::ComPtr<ID3D11Buffer>& targetBuf, ECBufBindType bindType = Graphics::ECBufBindType::ToAll);

private:
    wrl::ComPtr<ID3D11Device> pDevice= nullptr;
    wrl::ComPtr<ID3D11DeviceContext> pContext = nullptr;
    wrl::ComPtr<IDXGISwapChain> pSwap = nullptr;
    wrl::ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
    wrl::ComPtr<ID3D11Buffer> pFrameConstantBuffer = nullptr;
    wrl::ComPtr<ID3D11Buffer> pObjectConstantBuffer = nullptr;
    wrl::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;
    wrl::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
    wrl::ComPtr<ID3D11InputLayout> pInputLayout = nullptr;

#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif
    HWND hWnd;
};
