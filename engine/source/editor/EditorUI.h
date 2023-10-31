#pragma once

#include "core/math/EngineMath.h"
#include "editor.h"
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include "ui/WindowUI.h"
#include "core/base/macro.h"
#include "EditorGlobalContext.h"

class GameObject;
namespace TinyEngine
{
    class Editor;

    class EditorUI : public WindowUI
    {
    private:
        // void        moveEntity(float     new_mouse_pos_x,
        //                        float     new_mouse_pos_y,
        //                        float     last_mouse_pos_x,
        //                        float     last_mouse_pos_y,
        //                        Matrix4x4 object_matrix);
        // void        updateCursorOnAxis(Vector2 cursor_uv);
        // void        buildEditorFileAssstsUITree(Edi?torFileNode* node);
        // void        processEditorCommand();
        void        drawAxisToggleButton(const char* string_id, bool check_state, EditorAxisMode axis_mode);
        // void        createComponentUI(Reflection::ReflectionInstance& instance);
        // void        createLeafNodeUI(Reflection::ReflectionInstance& instance);
        // std::string getLeafUINodeParentLabel();
        
        void showEditorUI();
        void showEditorMenu(bool* p_open);
        void showEditorHierarchysWindow(bool* p_open);
        void showEditorProjectWindow(bool* p_open);
        void showEditorGameWindow(bool* p_open);
        void showEditorInspectorWindow(bool* p_open);
        void buildHierarchysUITree(GameObject* node);
        void onReset();

        std::string getLeafUINodeParentLabel();
        void     onDeleteSelectedGameObject();

    public:
        EditorUI(Editor* editor);
        EditorUI::~EditorUI();
        bool OnTick(UIState* uistate) override;
        
        void draw_frame() override;
        void clear() override;
    
    private:
        Editor* m_editor {nullptr};

        std::unordered_map<std::string, std::function<void(std::string, void*)>> m_editor_ui_creator;
        std::unordered_map<std::string, unsigned int>                            m_new_object_index_map;


    };
} // namespace Pilot
