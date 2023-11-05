#pragma once

namespace TinyEngine
{
    struct RuntimeGlobalContextInitInfo
    {
        // class RenderSystem* render_system;
        class Graphics* graphics;
    };

    struct ShadowConfig
    {
        struct point_shadow_config {
            int depth_bias{40};
            float slope_scaled_bias{2.5f};
            float depth_bias_clamp{1.0f};
        } m_point_shadow_config;
    };

    class RuntimeGlobalContext
    {
    public:
        class Graphics*     m_graphics {nullptr};
        struct ShadowConfig m_shadow_config{};
    public:
        void initialize(const RuntimeGlobalContextInitInfo& init_info);
        void clear();
    };
    
    extern RuntimeGlobalContext g_runtime_global_context;
}