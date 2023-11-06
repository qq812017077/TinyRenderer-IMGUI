#include "RuntimeGlobalContext.h"

namespace TinyEngine
{
    RuntimeGlobalContext g_runtime_global_context;

    void RuntimeGlobalContext::initialize(const RuntimeGlobalContextInitInfo& init_info)
    {
        // m_graphics = init_info.graphics;
        m_skybox_cubemap_path = {
            "res/images/skybox/2/0.png",
            "res/images/skybox/2/1.png",
            "res/images/skybox/2/2.png",
            "res/images/skybox/2/3.png",
            "res/images/skybox/2/4.png",
            "res/images/skybox/2/5.png",
        };
    }
    void RuntimeGlobalContext::clear()
    {
        // m_graphics = nullptr;
    }
}