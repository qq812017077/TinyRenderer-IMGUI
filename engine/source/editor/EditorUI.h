#pragma once

#include "core/math/EngineMath.h"
#include "editor.h"
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include "surfaces/SurfaceUI.h"

namespace TinyEngine
{
    class Editor;
    class GameObject;
    enum class EditorAxisMode : int
    {
        TranslateMode = 0,
        RotateMode    = 1,
        ScaleMode     = 2,
        Default       = 3
    };

    class EditorUI : public SurfaceUI
    {
    private:
        // void        onFileContentItemClicked(EditorFileNode* node);
        // void        drawSelectedEntityAxis();
        // void        moveEntity(float     new_mouse_pos_x,
        //                        float     new_mouse_pos_y,
        //                        float     last_mouse_pos_x,
        //                        float     last_mouse_pos_y,
        //                        Matrix4x4 object_matrix);
        // void        updateCursorOnAxis(Vector2 cursor_uv);
        // void        buildEditorFileAssstsUITree(Edi?torFileNode* node);
        // void        processEditorCommand();
        // void        drawAxisToggleButton(const char* string_id, bool check_state, EditorAxisMode axis_mode);
        // void        createComponentUI(Reflection::ReflectionInstance& instance);
        // void        createLeafNodeUI(Reflection::ReflectionInstance& instance);
        // std::string getLeafUINodeParentLabel();
        
        void showEditorUI();
        void showEditorMenu(bool* p_open);
        void showEditorWorldObjectsWindow(bool* p_open);
        void showEditorFileContentWindow(bool* p_open);
        void showEditorGameWindow(bool* p_open);
        void showEditorDetailWindow(bool* p_open);
        
        void onReset();

        GameObject* getSelectedGameObject() const;
        void     onGameObjectSelected(size_t selected_gobject_id);
        void     onDeleteSelectedGameObject();

    public:
        EditorUI(Editor* editor);
        EditorUI::~EditorUI();
        bool OnTick(UIState* uistate) override;
        void draw_ui() override;
        void clear() override;
    private:
        Editor* m_editor {nullptr};

        std::unordered_map<std::string, std::function<void(std::string, void*)>> m_editor_ui_creator;
        std::unordered_map<std::string, unsigned int>                            m_new_object_index_map;

        bool m_is_editor_mode {true};
        int  m_key_state {0};

        // 0 for x, 1 for y, 2 for z
        // 0 for yoz, 1 for xoz, 2 for xoy
        size_t    m_selected_gobject_id {0};
        Matrix4x4 m_selected_object_matrix {Matrix4x4::Identity()};

    };
} // namespace Pilot
