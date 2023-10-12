#pragma once
#include <memory>

class GameObject;
namespace TinyEngine
{
    class Scene;
    enum class PRenderPath : int
    {
        Forward,
        Deferred,
        Clustered
    };

    class UIState
    {
        PRenderPath m_renderPath {PRenderPath::Forward};

        bool m_fullscreen {false};
        bool m_showUI {true};
        bool m_showConfigWindow {true};
        bool m_showLog {false};

        struct
        {
            bool fps;
            bool frameTime;
            bool profiler;
            bool gpuMemory;
        } m_overlays {true, true, true, true};

        // std::shared_ptr<PCamera> m_editor_camera;
    };
    class FrameBuffer
    {
    public:
        FrameBuffer() : m_uistate(std::make_unique<UIState>()) {}
        size_t logicalFrameIndex {0};

        PRenderPath              m_renderpath {PRenderPath::Forward};
        std::shared_ptr<Scene>   m_scene;
        std::unique_ptr<UIState> m_uistate;
        GameObject*             m_selectedObject {nullptr};
    };
}