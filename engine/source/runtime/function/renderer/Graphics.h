#pragma once

class Graphics
{
protected:
    Graphics();

    virtual void CreateDevice() = 0;
    virtual void CreateRenderTarget() = 0;
    virtual void ClearupDevice() = 0;
    virtual void ClearupRenderTarget() = 0;

public:
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics();
    virtual void EndFrame() = 0;
    virtual void ClearBuffer(float red, float green, float blue) noexcept = 0;

    virtual void DrawTestTriangle() = 0;
    
};