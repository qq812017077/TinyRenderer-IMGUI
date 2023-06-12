#pragma once
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

struct GraphicsInfo
{
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    IDXGISwapChain* pSwap = nullptr;
    ID3D11RenderTargetView* pTarget = nullptr;
};