#include "Graphics.h"

Graphics::Graphics(WindowInfo &wnd)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 0u;
    sd.BufferDesc.Height = 0u;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0u;
    sd.BufferDesc.RefreshRate.Denominator = 0u;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1u;
    sd.SampleDesc.Quality = 0u;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1u;
    sd.OutputWindow = wnd.hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0u;
    

    D3D11CreateDeviceAndSwapChain(
        nullptr,                    // default adapter
        D3D_DRIVER_TYPE_HARDWARE,   // use hardware graphics driver
        nullptr,                    // no software device
        0u,                         // no flags
        nullptr,                    // default feature level array
        0u,                         // default feature level array size
        D3D11_SDK_VERSION,          // SDK version
        &sd,                        // swap chain description
        &info.pSwap,                     // swap chain
        &info.pDevice,                   // device
        nullptr,                    // supported feature level
        &info.pContext                   // device context
    );

    ID3D11Buffer *pBackBuffer = nullptr;
    info.pSwap->GetBuffer(0u, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    info.pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &info.pTarget);
}

Graphics::~Graphics()
{
    if (info.pTarget)
    {
        info.pTarget->Release();
    }
    if (info.pSwap)
    {
        info.pSwap->Release();
    }
    if (info.pContext)
    {
        info.pContext->Release();
    }
    if (info.pDevice)
    {
        info.pDevice->Release();
    }
}

void Graphics::EndFrame()
{
    HRESULT hr;
    if (FAILED(hr = info.pSwap->Present(1u, 0u)))
    {
        //if (hr == DXGI_ERROR_DEVICE_REMOVED)
        //{
        //    throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
        //}
        //else
        //{
        //    throw GFX_EXCEPT(hr);
        //}
    }
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
    const float color[] = { red, green, blue, 1.0f };
    info.pContext->ClearRenderTargetView(info.pTarget, color);
}
