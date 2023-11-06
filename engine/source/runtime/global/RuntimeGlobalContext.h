#pragma once
#include <string>
#include <vector>
namespace TinyEngine
{
    struct RuntimeGlobalContextInitInfo
    {
        // class RenderSystem* render_system;
        class Graphics* graphics;
    };

    struct ShadowConfig
    {
        struct directional_shadow_config {
            bool use_csm{true};
            int csm_split_num{4};
            float csm_split_lambda{0.5f};
        } m_directional_shadow_config;

        struct point_shadow_config {
            int depth_bias{40};
            float slope_scaled_bias{2.5f};
            float depth_bias_clamp{1.0f};
        } m_point_shadow_config;
    };

    class RuntimeGlobalContext
    {
    public:
        // class Graphics*             m_graphics {nullptr};
        struct ShadowConfig         m_shadow_config{};
        std::vector<std::string>    m_skybox_cubemap_path;
    public:
        void initialize(const RuntimeGlobalContextInitInfo& init_info);
        void clear();
    };
    
    extern RuntimeGlobalContext g_runtime_global_context;
}