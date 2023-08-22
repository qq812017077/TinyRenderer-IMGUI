#pragma once
#include "imgui/imgui.h"
#include "Win32Window.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>

class ImguiManager
{
public:
    ImguiManager();
    ~ImguiManager();

    void Init(Win32Window* pWindow);
    void NewFrame();
    void Render();

    void SetEnable(bool enable);
    bool IsEnabled();
    void OnDeviceLost();
    void OnDeviceRestored();

private:
    void CreateDeviceObjects();
    void InvalidateDeviceObjects();

    HWND hWnd;
    bool isEnabled = false;
    // ID3D11Device* pDevice;
    // ID3D11DeviceContext* pContext;
    // ID3D11Buffer* pVB;
    // ID3D11Buffer* pIB;
    // ID3D11VertexShader* pVertexShader;
    // ID3D11InputLayout* pInputLayout;
    // ID3D11Buffer* pVertexConstantBuffer;
    // ID3D11PixelShader* pPixelShader;
    // ID3D11SamplerState* pFontSampler;
    // ID3D11ShaderResourceView* pFontTextureView;
    // ID3D11RasterizerState* pRasterizerState;
    // ID3D11BlendState* pBlendState;
    // ID3D11DepthStencilState* pDepthStencilState;
    int vertexBufferSize = 5000, indexBufferSize = 10000;
};