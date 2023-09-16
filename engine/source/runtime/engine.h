#pragma once
#include "core/Singleton.h"
#include "core/Timer.h"
#include "surfaces/SurfaceUI.h"

class Graphics;
namespace TinyEngine
{
    class FrameBuffer;
    extern bool g_is_editor_mode;
    class Engine : public Singleton<Engine>
    {
        friend class Editor;
    public:
        Engine();
        ~Engine();

        void Initialize();
        void Run();
        void ShutdownEngine();
        
        void BindEditor(SurfaceUI* surface_ui) { m_surface_ui = surface_ui; }
    protected:
        float tick();
        void logicalTick(float deltaTime);
        bool rendererTick();

    private:
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        Timer timer;
        SurfaceUI* m_surface_ui = nullptr;
        FrameBuffer* m_framebuffer = nullptr;
        Graphics * m_graphics = nullptr;
    };
} // namespace TinyEngine