#pragma once
#include "core/math/EngineMath.h"
#include "components/components.h"

#include <mutex>
#include <vector>
namespace TinyEngine
{
    class BVHNode;
    class Effect;
    struct AmbientLight
    {
        Vector3 m_irradiance;
        float padding;
    };
    struct DirectionalLight
    {
        struct DirectionalLightBuffer
        {
            Vector3 m_direction;
            float padding;
            Vector3 m_color;
            float padding2;
        };
        DirectionalLightBuffer m_buffer;
        Matrix4x4 m_lightView;
        Matrix4x4 m_lightProj;
        Matrix4x4 m_lightViewProj;

        std::vector<Renderer *> visibleRenderers;
    };

    struct EffectDesc
    {
        Effect * pEffect;
        std::vector<Renderer *> renderers;
    };

    struct ShadowCastDesc
    {
        std::vector<Renderer *> renderers;
    };

    struct PointLight
    {
        struct PointLightBuffer
        {
            Vector3 pos;
            float padding;
            Vector4 color;
            float atten;
            float range;
        };
        PointLightBuffer m_buffer;
        Matrix4x4 m_lightView[6];
        Matrix4x4 m_lightProj;
        Matrix4x4 m_lightViewProj[6];

        std::vector<Renderer *> visibleRenderers;

    };

    struct CameraList
    {
        std::vector<Camera *> cameras;
    };
    class Scene
    {
    public:
        bool                    m_loaded = false;
        Vector3                 m_minBounds {0, 0, 0};
        Vector3                 m_maxBounds {0, 0, 0};
        Vector3                 m_center {0, 0, 0};
        float                   m_diagonal {0};
        
        CameraList              m_camera_list;
        Camera *                m_main_camera {nullptr};
        Camera *                selectedCamera {nullptr};
        AmbientLight            m_ambient_light;
        DirectionalLight        m_directional_light;
        std::vector<PointLight> m_point_lights;
        
        // all effectDesc
        std::vector<Renderer *>     m_renderers;
        std::vector<Renderer *>     selectedRenderers;
        std::vector<EffectDesc>     CamVisibleRenderers;

        Bounds scene_bounds;
        void                        Lock() { m_scene_mutex.lock(); }
        void                        Unlock() { m_scene_mutex.unlock(); }
        void                        Load();
        void                        Clear();
        void updateVisibleObjects(std::vector<EffectDesc> & effectDescs, std::vector<ShadowCastDesc> & ShadowCastDescs);
    private:
        void updateVisibleObjectsDirectionalLight(std::vector<ShadowCastDesc> & ShadowCastDescs);
        void updateVisibleObjectsPointLight(std::vector<ShadowCastDesc> & ShadowCastDescs);
        // void updateVisibleObjectsSpotLight();
        void updateVisibleObjectsMainCamera(std::vector<EffectDesc> & effectDescs);

        // std::shared_ptr<BVHNode> m_bvh_root;
        

        Matrix4x4 CalculateDirectionalLightCamera();

        std::mutex m_scene_mutex;
    };
}