#pragma once
#include "core/Singleton.h"
#include "core/math/EngineMath.h"
#include <memory>
#include "Window.h"
#include <memory>
#include "ImguiManager.h"

namespace TinyEngine
{
    class Engine;
    class EditorUI;
    class Editor : public Singleton<Editor>
    {
        friend class EditorUI;
        friend class EngineRenderer;
    public:
        Editor();
        virtual ~Editor();

        void Initialize(Engine* engine_runtime);
        int Run();
        void Clear();

    protected:
        // engine interface
        void   onWindowChanged(float pos_x, float pos_y, float width, float height) const;
        size_t onUpdateCursorOnAxis(int axis_mode, const Vector2& cursor_uv, const Vector2& window_size) const;
        size_t getGuidOfPickedMesh(const Vector2& picked_uv) const;

    protected:
        std::shared_ptr<EditorUI> m_editor_ui;
        Engine*              m_engine_runtime {nullptr};

        std::unique_ptr<Window> pWnd;
        ImguiManager imgui;
    };
} // namespace Pilot
