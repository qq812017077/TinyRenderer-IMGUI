#include "EditorGlobalContext.h"
#include "Graphics.h"
namespace TinyEngine
{
    EditorGlobalContext g_editor_global_context;

    void EditorGlobalContext::initialize(const EditorGlobalContextInitInfo& init_info)
    {
        g_editor_global_context.m_window_system  = init_info.window_system;
        // g_editor_global_context.m_render_system  = init_info.render_system;
        g_editor_global_context.m_engine_runtime = init_info.engine_runtime;
        g_editor_global_context.m_graphics       = init_info.graphics;
        m_scene_manager = new EditorSceneManager();
        m_input_manager = new EditorInputManager();
        m_scene_manager->initialize();
        m_input_manager->initialize();
    }

    void EditorGlobalContext::clear()
    {
        delete (m_scene_manager);
        delete (m_input_manager);
    }

    
    void EditorGlobalContext::onWindowChanged(Vector2 pos, Vector2 size) const{
        m_graphics->UpdateRenderSceneViewPort(pos.x, pos.y, size.x, size.y);
        m_input_manager->setEngineWindowPos(pos);
        m_input_manager->setEngineWindowSize(size);
    }
}