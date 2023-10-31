#include "EditorSceneManager.h"
#include "ui/GUI.h"
#include "object/GameObject.h"
#include "world/WorldManager.h"
#include "scene/SceneManager.h"

#include "EditorGlobalContext.h"
#include "Graphics.h"

namespace TinyEngine
{
    void EditorSceneManager::initialize()
    {
        
    }

    void EditorSceneManager::tick(float delta_time)
    {
        
    }

    

    void EditorSceneManager::drawSelectedEntityAxis()
    {
        GameObject* selected_object = getSelectedGameObject();

        if (m_is_editor_mode && selected_object != nullptr)
        {
            auto pTrans = selected_object->GetComponent<Transform>();
            // std::vector<RenderMesh>   axis_meshs;

            Vector3    scale = pTrans->GetLossyScale();
            Quaternion rotation = pTrans->GetRotation();
            Vector3    translation = pTrans->GetPosition();
            Matrix4x4 translation_matrix = Matrix4x4::Translation(translation);
            Matrix4x4 scale_matrix       = Matrix4x4::Scale(scale);
            Matrix4x4 axis_model_matrix  = translation_matrix * scale_matrix;
            if (m_axis_mode == EditorAxisMode::TranslateMode)
            {
                // m_translation_axis.m_model_matrix = axis_model_matrix;
                // axis_meshs.push_back(m_translation_axis);
                // SceneManager::Get().setAxisMesh(axis_meshs);
            }
            else if (m_axis_mode == EditorAxisMode::RotateMode)
            {
                // m_rotation_axis.m_model_matrix = axis_model_matrix;
                // axis_meshs.push_back(m_rotation_axis);
                // SceneManager::getInstance().setAxisMesh(axis_meshs);
            }
            else if (m_axis_mode == EditorAxisMode::ScaleMode)
            {
                // axis_model_matrix           = axis_model_matrix * Matrix4x4(rotation);
                // m_scale_aixs.m_model_matrix = axis_model_matrix;
                // axis_meshs.push_back(m_scale_aixs);
                // SceneManager::getInstance().setAxisMesh(axis_meshs);
            }
        }
        else
        {
            // std::vector<RenderMesh> axis_meshs;
            // SceneManager::getInstance().setAxisMesh(axis_meshs);
        }
    }

    GameObject * EditorSceneManager::getSelectedGameObject() const
    {
        if(m_selected_gobject_id == INVALID_GO_ID)
            return nullptr;
        auto pLevel = WorldManager::Get().GetCurrentActiveLevel();
        if(pLevel)
        {
            return pLevel->GetGameObjectByID(m_selected_gobject_id);
        }
        return nullptr;
    }

    
    size_t EditorSceneManager::PickGuidOfGameObject(Vector2 cursor_uv)
    {
        return g_editor_global_context.m_graphics->PickGuidOfGameObject(cursor_uv.x, cursor_uv.y);
    }
 
    void EditorSceneManager::onGameObjectSelected(size_t selected_gobject_id)
    {
        if (selected_gobject_id == m_selected_gobject_id)
            return;

        m_selected_gobject_id = selected_gobject_id;

        auto pLevel = WorldManager::Get().GetCurrentActiveLevel();
        GameObject* selected_gobject = getSelectedGameObject();

        SceneManager::Get().SetSelectedGameObject(selected_gobject);
        // drawSelectedEntityAxis();

        if (m_selected_gobject_id != INVALID_GO_ID)
        {
            LOG_INFO("select game object " + std::to_string(m_selected_gobject_id));
        }
        else
        {
            LOG_INFO("no game object selected");
        }
    }
}