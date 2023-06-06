#pragma once
#include "EngineWin.h"
#include <d3d11.h>

class Graphics
{
public:
    Graphics(HWND hWnd);
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics();
    void EndFrame();
    void ClearBuffer(float red, float green, float blue) noexcept;
private:
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    IDXGISwapChain* pSwap = nullptr;
    ID3D11RenderTargetView* pTarget = nullptr;
};
