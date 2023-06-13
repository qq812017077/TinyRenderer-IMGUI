#pragma once
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#ifndef NDEBUG
#include "DxgiInfoManager.h"
#endif

#include <wrl.h>

struct GraphicsInfo
{
    Microsoft::WRL::ComPtr<ID3D11Device> pDevice= nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext = nullptr;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
    // ID3D11DeviceContext* pContext = nullptr;
    // IDXGISwapChain* pSwap = nullptr;
    // ID3D11RenderTargetView* pTarget = nullptr;
#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif
};