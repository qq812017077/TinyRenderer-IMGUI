#pragma once

namespace TinyEngine
{
    struct RuntimeGlobalContextInitInfo
    {
        // class RenderSystem* render_system;
        class Graphics* graphics;
    };

    class RuntimeGlobalContext
    {
    public:
        class Graphics*     m_graphics {nullptr};
    
    public:
        void initialize(const RuntimeGlobalContextInitInfo& init_info);
        void clear();
    };
    
    extern RuntimeGlobalContext g_runtime_global_context;
}