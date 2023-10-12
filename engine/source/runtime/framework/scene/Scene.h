#pragma once
#include "core/math/EngineMath.h"
#include "components/components.h"

#include <mutex>
#include <vector>
namespace TinyEngine
{
    class Effect;
    struct AmbientLight
    {
        Vector3 m_irradiance;
        float padding;
    };
    struct DirectionalLight
    {
        Vector3 m_direction;
        float padding;
        Vector3 m_color;
    };

    struct EffectDesc
    {
        Effect * pEffect;
        std::vector<Renderer *> renderers;
    };

    struct PointLight
    {
        Vector3 pos;
        float padding;
        Vector4 color;
        float atten;
        float range;
        // float kConstant;
        // float kLinear;
        // float kQuadratic;
    };

    class Scene
    {
    public:
        bool                    m_loaded = false;
        Vector3                 m_minBounds {0, 0, 0};
        Vector3                 m_maxBounds {0, 0, 0};
        Vector3                 m_center {0, 0, 0};
        float                   m_diagonal {0};
        
        Camera *                m_main_camera {nullptr};
        AmbientLight            m_ambient_light;
        DirectionalLight        m_directional_light;
        std::vector<PointLight> m_point_lights;

        // all effectDesc
        std::vector<EffectDesc> effectDescs;

        std::vector<Renderer *> selectedRenderers;
        void                        Lock() { m_scene_mutex.lock(); }
        void                        Unlock() { m_scene_mutex.unlock(); }
        void                        Load();
        void                        Clear();
        
    private:
        std::mutex m_scene_mutex;
    };
}