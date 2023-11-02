#include "RuntimeGlobalContext.h"

namespace TinyEngine
{
    RuntimeGlobalContext g_runtime_global_context;

    void RuntimeGlobalContext::initialize(const RuntimeGlobalContextInitInfo& init_info)
    {
        m_graphics = init_info.graphics;
    }
    void RuntimeGlobalContext::clear()
    {
        m_graphics = nullptr;
    }
}