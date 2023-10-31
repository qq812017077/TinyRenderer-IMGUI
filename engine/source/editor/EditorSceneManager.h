#pragma once
#include "Matrix.h"
#include "core/base/macro.h"

class GameObject;
namespace TinyEngine
{
    enum class EditorAxisMode : int
    {
        TranslateMode = 0,
        RotateMode = 1,
        ScaleMode = 2,
        Default = 3
    };


    class EditorSceneManager
    {
    public:
        void initialize();
        void tick(float delta_time);

        void drawSelectedEntityAxis();

        // RenderCamera * getEditorCamera() const { return m_editor_camera; }
        EditorAxisMode getEditorAxisMode() { return m_axis_mode; }
        void setEditorAxisMode(EditorAxisMode new_axis_mode) { m_axis_mode = new_axis_mode; }
        GameObject* getSelectedGameObject() const;
        size_t PickGuidOfGameObject(Vector2 cursor_uv);
        size_t getSelectedObjectID() const { return m_selected_gobject_id; }
        void     onGameObjectSelected(size_t selected_gobject_id);
    private:
    private:
        // RenderCamera * m_editor_camera{ nullptr };
        bool m_is_editor_mode {true};
        EditorAxisMode m_axis_mode{ EditorAxisMode::TranslateMode };
        size_t    m_selected_gobject_id {INVALID_GO_ID};
        Matrix4x4 m_selected_object_matrix {Matrix4x4::Identity()};
    };
}