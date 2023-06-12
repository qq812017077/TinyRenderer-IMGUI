#pragma once
#include "EngineException.h"
#include "GraphicsInfo.h"
#include "WindowInfo.h"

#define GFX_THROW_FAILED(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::GraphicsException(__LINE__, __FILE__, hr)

class Graphics
{
public:
    class GraphicsException : public EngineException
    {
        public:
            GraphicsException(int line, const char* file, const char* msg) noexcept : EngineException(line, file, msg){}
        private:

    };
public:
    Graphics(WindowInfo &wnd);
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics();
    void EndFrame();
    void ClearBuffer(float red, float green, float blue) noexcept;

private:
    GraphicsInfo info;
};

