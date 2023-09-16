#pragma once
#include "core/math/EngineMath.h"
#include "components/components.h"

#include <mutex>
namespace TinyEngine
{
    class Scene
    {
    public:
        bool                     m_loaded = false;
        Vector3                  m_minBounds {0, 0, 0};
        Vector3                  m_maxBounds {0, 0, 0};
        Vector3                  m_center {0, 0, 0};
        float                    m_diagonal {0};
        Camera *                 m_main_camera {nullptr};
        
        void                        Lock() { m_scene_mutex.lock(); }
        void                        Unlock() { m_scene_mutex.unlock(); }
        void                        Load();
        void                        Clear();

    private:
        std::mutex m_scene_mutex;
    };
}