#include "Graphics.h"
#include "Exceptions.h"
// add ComPtr
namespace wrl = Microsoft::WRL;



HRESULT hr;

Graphics::Graphics(WindowInfo &wnd)
{
    CreateDevice(wnd, graphicsInfo);
    CreateRenderTarget(graphicsInfo);
}

Graphics::~Graphics()
{
    ClearupDevice(graphicsInfo);
}

void Graphics::EndFrame()
{
#ifndef NDEBUG
    auto &infoManager = graphicsInfo.infoManager;
    infoManager.Set();
#endif
    HRESULT hr;
    if (FAILED(hr = graphicsInfo.pSwap->Present(1u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw GFX_DEVICE_REMOVED_EXCEPT(graphicsInfo.pDevice->GetDeviceRemovedReason());
        }
        else
        {
            throw GFX_EXCEPT(hr);   
        }
    }
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
    const float color[] = { red, green, blue, 1.0f };
    graphicsInfo.pContext->ClearRenderTargetView(graphicsInfo.pTarget.Get(), color);
}

void Graphics::CreateDevice(WindowInfo &wnd, GraphicsInfo &graphicsInfo)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2u;
    sd.BufferDesc.Width = 0u;
    sd.BufferDesc.Height = 0u;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0u;
    sd.BufferDesc.RefreshRate.Denominator = 0u;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = wnd.hWnd;
    sd.SampleDesc.Count = 1u;
    sd.SampleDesc.Quality = 0u; // (HWND)1234567;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
#ifndef NDEBUG
    sd.Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    auto &infoManager = graphicsInfo.infoManager;
    GFX_THROW_INFO(
        D3D11CreateDeviceAndSwapChain(
        nullptr,                    // default adapter
        D3D_DRIVER_TYPE_HARDWARE,   // use hardware graphics driver
        nullptr,                    // no software device
        0u,                         // no flags
        nullptr,                    // default feature level array
        0u,                         // default feature level array size
        D3D11_SDK_VERSION,          // SDK version
        &sd,                        // swap chain description
        &graphicsInfo.pSwap,                     // swap chain
        &graphicsInfo.pDevice,                   // device
        nullptr,                    // supported feature level
        &graphicsInfo.pContext                   // device context
    ));
}

void Graphics::CreateRenderTarget(GraphicsInfo &info)
{
    auto &infoManager = info.infoManager;
    wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
    GFX_THROW_INFO(info.pSwap->GetBuffer(0u, __uuidof(ID3D11Texture2D), &pBackBuffer));
    GFX_THROW_INFO(info.pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &info.pTarget));
}

void Graphics::ClearupDevice(GraphicsInfo &info)
{
    ClearupRenderTarget(info);
}

void Graphics::ClearupRenderTarget(GraphicsInfo &info)
{

}