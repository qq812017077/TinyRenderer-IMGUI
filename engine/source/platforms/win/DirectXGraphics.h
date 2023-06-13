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


class DirectXGraphics : public Graphics
{
public:
    DirectXGraphics(HWND &hwnd);
    DirectXGraphics(const DirectXGraphics&) = delete;
    DirectXGraphics& operator=(const DirectXGraphics&) = delete;
    ~DirectXGraphics();
    void EndFrame() override;
    void ClearBuffer(float red, float green, float blue) noexcept override;
    void DrawTestTriangle() override;
protected:
    void CreateDevice() override;
    void CreateRenderTarget() override;
    void ClearupDevice() override;
    void ClearupRenderTarget() override;
private:

    Microsoft::WRL::ComPtr<ID3D11Device> pDevice= nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext = nullptr;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif
    HWND hWnd;
};
