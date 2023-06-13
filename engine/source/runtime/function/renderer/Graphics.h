#pragma once
#include "GraphicsInfo.h"
#include "WindowInfo.h"

class Graphics
{
public:
    Graphics(WindowInfo &wnd);
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics();
    void EndFrame();
    void ClearBuffer(float red, float green, float blue) noexcept;

private:
    GraphicsInfo graphicsInfo;

    static void CreateDevice(WindowInfo &wndInfo, GraphicsInfo &graphicsInfo);
    static void CreateRenderTarget(GraphicsInfo &info);
    static void ClearupDevice(GraphicsInfo &info);
    static void ClearupRenderTarget(GraphicsInfo &info);
};

