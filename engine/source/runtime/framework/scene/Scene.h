#pragma once
#include "core/math/EngineMath.h"
#include "components/components.h"
#include "RenderEntity.h"
#include <mutex>
#include <vector>
#include <array>
#include "RenderContext.h"

namespace TinyEngine
{
    class BVHNode;
    class Effect;


    struct AmbientLight
    {
        Vector3 m_irradiance;
        float padding;
    };

    struct NSM  // normal shadow map
    {
        
        Matrix4x4 m_view_proj;
        std::vector<Renderer *> visibleRenderers;
    }; 

    struct CSM  // Cascaded Shadow Map
    {
        int m_split_num{4};
        float m_split_lambda{0.5f};

        // based on split_num
        std::vector<Vector2> m_split_depth;
        std::vector<Matrix4x4> m_split_view_proj;
        std::vector<std::vector<Renderer *>> m_split_visible_renderers;
        Vector4 m_far_planes; // far planes after perspective divide
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
        bool exist;
        DirectionalLightBuffer m_buffer;
        bool use_csm{true};
        NSM m_nsm;
        CSM m_csm;
    };

    struct EffectDesc
    {
        Effect * pEffect;
        std::vector<Renderer *> renderers;
        std::vector<RenderEntity> renderEntities;
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

        SceneResource*          p_map_resource;
        CameraList              m_camera_list;
        Camera *                m_main_camera {nullptr};
        Camera *                selectedCamera {nullptr};
        AmbientLight            m_ambient_light;
        DirectionalLight        m_directional_light;
        std::vector<PointLight> m_point_lights;
        
        // all effectDesc
        std::vector<Renderer *>     shadowCastRenderers;
        std::vector<Renderer *>     m_renderers;
        std::vector<Renderer *>     selectedRenderers;
        std::vector<EffectDesc>     CamVisibleRenderers;

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
        void splitingViewFrustum(std::vector<Vector2>& split_depth, int split_num, float lambda);
        void updateLightViewProjMatrix(std::vector<Matrix4x4>& split_view_projs, std::vector<Vector2>& split_depth);
        std::mutex m_scene_mutex;
    };
}