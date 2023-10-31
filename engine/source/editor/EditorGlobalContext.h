#pragma once
#include "EditorSceneManager.h"
#include "EditorInputManager.h"
class Graphics;
class Window;
namespace TinyEngine
{
    class Engine;
    struct EditorGlobalContextInitInfo
    {
        // class RenderSystem* render_system;
        class Graphics* graphics;
        class Window* window_system;
        class Engine*  engine_runtime;
    };

    class EditorGlobalContext
    {
    public:
        class EditorSceneManager* m_scene_manager {nullptr};
        class EditorInputManager* m_input_manager {nullptr};
        class Graphics*     m_graphics {nullptr};
        // class RenderSystem*       m_render_system {nullptr};
        class Window*       m_window_system {nullptr};
        class Engine*        m_engine_runtime {nullptr};

        bool m_is_editor_mode {true};

    public:
        void initialize(const EditorGlobalContextInitInfo& init_info);
        void clear();
        void onWindowChanged(Vector2 pos, Vector2 size) const;
    };

    extern EditorGlobalContext g_editor_global_context;
} // namespace Piccolo