#pragma once
#include "core/Singleton.h"
#include "core/Timer.h"
#include "ui/WindowUI.h"

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
        float tick();
        bool tickOneFrame(float deltaTime);
        void ShutdownEngine();
        
        void BindEditor(WindowUI* surface_ui) { m_surface_ui = surface_ui; }
    protected:
        void logicalTick(float deltaTime);
        bool rendererTick();

    private:
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        Timer timer;
        WindowUI* m_surface_ui = nullptr;
        FrameBuffer* m_framebuffer = nullptr;
        Graphics * m_graphics = nullptr;
    };
} // namespace TinyEngine