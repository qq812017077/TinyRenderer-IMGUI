#include "EditorUI.h"
#include "ui/GUI.h"
#include "Window.h"
#include "world/WorldManager.h"
#include "object/GameObject.h"
#include "scene/SceneManager.h"
namespace TinyEngine
{
    EditorUI::EditorUI(Editor* editor)
    {
        m_editor = editor;
    }

    EditorUI::~EditorUI()
    {
    }

    bool EditorUI::OnTick(UIState* uistate)
    {
        m_editor->imgui.NewFrame();
        
        // Print the information to the console
        if (const auto ecode = m_editor->pWnd->ProcessMessages()) return false;
        
        showEditorUI();
        processEditorCommand();
        draw_frame();
        return true;
    }

    void EditorUI::clear()
    {
        
    }

    void EditorUI::draw_frame()
    {
        // static bool show_demo_window = true;
        // ImGui::ShowDemoWindow(&show_demo_window);

        // for (auto& pGo : GameObject::GetRootGameObjects()) pGo->OnGUI();
        // if(ImGui::Begin("FPS"))
        // {
        //     ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
        //         1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        // }
        // ImGui::End();
        m_editor->imgui.Render();
    }

    void EditorUI::showEditorUI()
    {
        bool editor_menu_window_open       = true;
        bool hierarchy_window_open         = true;
        bool game_engine_window_open       = true;
        bool project_content_window_open      = true;
        bool inspector_window_open            = true;
        bool scene_lights_window_open      = true;
        bool scene_lights_data_window_open = true;

        showEditorMenu(&editor_menu_window_open);
        showEditorHierarchysWindow(&hierarchy_window_open);
        showEditorGameWindow(&game_engine_window_open);
        showEditorProjectWindow(&project_content_window_open);
        showEditorInspectorWindow(&inspector_window_open);
    }
    void EditorUI::showEditorMenu(bool* p_open)
    {
        float menu_height = 18.0f;
        float window_width = static_cast<float>(m_editor->pWnd->GetWidth());
        float window_height = static_cast<float>(m_editor->pWnd->GetHeight());
        ImGuiDockNodeFlags dock_flags   = ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoTabBar ;
        ImGuiWindowFlags   window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
                                        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                        ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(main_viewport->WorkPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_Always);
        ImGui::SetNextWindowViewport(main_viewport->ID);
        ImGui::Begin("Editor menu", p_open, window_flags);

        ImGuiID main_docking_id = ImGui::GetID("Main Docking");
        if (ImGui::DockBuilderGetNode(main_docking_id) == nullptr)
        {
            ImGui::DockBuilderRemoveNode(main_docking_id);

            ImGui::DockBuilderAddNode(main_docking_id, dock_flags);
            ImGui::DockBuilderSetNodePos(main_docking_id,
                                         ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + menu_height));
            
            ImGuiID center = main_docking_id;
            ImGuiID left;
            ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.25f, nullptr, &left);

            ImGuiID left_other;
            ImGuiID left_file_content = ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.30f, nullptr, &left_other);

            ImGuiID left_game_engine;
            ImGuiID left_asset = ImGui::DockBuilderSplitNode(left_other, ImGuiDir_Left, 0.30f, nullptr, &left_game_engine);
            
            ImGui::DockBuilderDockWindow("Hierarchy", left_asset);
            ImGui::DockBuilderDockWindow("File Content", left_file_content);
            ImGui::DockBuilderDockWindow("Game Engine", left_game_engine);
            ImGui::DockBuilderDockWindow("Inspector", right);

            ImGui::DockBuilderFinish(main_docking_id);
        }

        ImGui::DockSpace(main_docking_id);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                if (ImGui::MenuItem("Reload Current Level"))
                {
                    // show dialog to show: "Wait to implement"
                    // ImGui::OpenPopup("Wait to implement");

                    // WorldManager::getInstance().reloadCurrentLevel();
                    // onGObjectSelected(PILOT_INVALID_GOBJECT_ID);
                }
                if (ImGui::MenuItem("Save Current Level"))
                {
                    // ImGui::OpenPopup("Wait to implement");
                    // WorldManager::getInstance().saveCurrentLevel();
                }
                if (ImGui::MenuItem("Exit"))
                {
                    exit(0);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }

    void EditorUI::showEditorHierarchysWindow(bool* p_open)
    {
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&window_class);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        if (!ImGui::Begin("Hierarchy", p_open, window_flags))
        {
            ImGui::End();
            return;
        }
        const Level* current_active_level = WorldManager::Get().GetCurrentActiveLevel();
        if (current_active_level == nullptr)
            return;

        const auto& all_gobjects = current_active_level->GetAllGameObjects();
        for (auto& id_object_pair : all_gobjects)
        {
            const size_t      object_id = id_object_pair.first;
            GameObject*        pGO    = id_object_pair.second;
            const std::string name    = pGO->GetName();
            if (name.size() > 0)
            {
                if (ImGui::Selectable(name.c_str(), m_selected_gobject_id == object_id))
                {
                    if (m_selected_gobject_id != object_id)
                    {
                        // onGObjectSelected(object_id);
                    }
                    else
                    {
                        // onGObjectSelected(PILOT_INVALID_GOBJECT_ID);
                    }
                    break;
                }
            }
        }
        ImGui::End();
    }

    void EditorUI::showEditorProjectWindow(bool* p_open)
    {
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton  | ImGuiDockNodeFlags_NoCloseButton;
        ImGui::SetNextWindowClass(&window_class);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        if (!ImGui::Begin("File Content", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH |
                                       ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg |
                                       ImGuiTableFlags_NoBordersInBody;

        if (ImGui::BeginTable("File Content", 2, flags))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();
            // Pilot::EditorFileService editor_file_service;
            // editor_file_service.buildEngineFileTree();
            // EditorFileNode* editor_root_node = editor_file_service.getEditorRootNode();
            // buildEditorFileAssstsUITree(editor_root_node);
            ImGui::EndTable();
        }
        // file image list

        ImGui::End();
    }

    void EditorUI::showEditorGameWindow(bool* p_open)
    {
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        ImGui::SetNextWindowClass(&window_class);
        const
        ImGuiIO&         io           = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        if (!ImGui::Begin("Game Engine", p_open, window_flags))
        {
            ImGui::End();
            return;
        }
        static bool trans_button_ckecked  = false;
        static bool rotate_button_ckecked = false;
        static bool scale_button_ckecked  = false;
        
        // switch (m_axis_mode)
        // {
        //     case EditorAxisMode::TranslateMode:
        //         trans_button_ckecked  = true;
        //         rotate_button_ckecked = false;
        //         scale_button_ckecked  = false;
        //         break;
        //     case EditorAxisMode::RotateMode:
        //         trans_button_ckecked  = false;
        //         rotate_button_ckecked = true;
        //         scale_button_ckecked  = false;
        //         break;
        //     case EditorAxisMode::ScaleMode:
        //         trans_button_ckecked  = false;
        //         rotate_button_ckecked = false;
        //         scale_button_ckecked  = true;
        //         break;
        //     default:
        //         break;
        // }

        // if (ImGui::BeginMenuBar())
        // {
        //     ImGui::Indent(10.f);
        //     drawAxisToggleButton("Trans", trans_button_ckecked, EditorAxisMode::TranslateMode);
        //     ImGui::Unindent();

        //     ImGui::SameLine();

        //     drawAxisToggleButton("Rotate", rotate_button_ckecked, EditorAxisMode::RotateMode);

        //     ImGui::SameLine();

        //     drawAxisToggleButton("Scale", scale_button_ckecked, EditorAxisMode::ScaleMode);

        //     ImGui::SameLine();

        //     float indent_val = 0.0f;
        //     indent_val       = m_engine_window_size.x - 100.0f;
        //     ImGui::Indent(indent_val);
        //     if (m_is_editor_mode)
        //     {
        //         ImGui::PushID("Editor Mode");
        //         ImGui::Button("Editor Mode");
        //         if (ImGui::IsItemClicked(0))
        //         {
        //             m_is_editor_mode = !m_is_editor_mode;
        //             drawSelectedEntityAxis();
        //             g_is_editor_mode = false;
        //             m_io->setFocusMode(true);
        //         }
        //         ImGui::PopID();
        //     }
        //     else
        //     {
        //         ImGui::Button("Game Mode");
        //         if (ImGui::IsItemClicked(0))
        //         {
        //             m_is_editor_mode = !m_is_editor_mode;
        //             g_is_editor_mode = true;
        //             SceneManager::getInstance().setMainViewMatrix(m_tmp_uistate->m_editor_camera->getViewMatrix());
        //         }
        //     }
        //     m_io->setEditorMode(m_is_editor_mode);
        //     ImGui::Unindent();
        //     ImGui::EndMenuBar();
        // }

        // if (!m_is_editor_mode)
        // {
        //     ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Press Left Alt key to display the mouse cursor!");
        // }

        Vector2 new_window_pos  = {0.0f, 0.0f};
        Vector2 new_window_size = {0.0f, 0.0f};
        
        // 获取当前dockWindow的pos和size
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        ImVec2 content = ImGui::GetContentRegionAvail();
        new_window_pos.x        = pos.x;
        new_window_pos.y        = pos.y + 38.0f;
        new_window_size.x       = size.x;
        new_window_size.y       = size.y - 38.0f;
        // 在dockWindow内部显示pos和size
        ImGui::Text("Pos: (%.2f, %.2f)", pos.x, pos.y);
        ImGui::Text("Size: (%.2f, %.2f)", size.x, size.y);
        ImGui::Text("Content: (%.2f, %.2f)", content.x, content.y);

        if (new_window_pos != m_engine_window_pos || new_window_size != m_engine_window_size)
        {
            m_editor->onWindowChanged(new_window_pos.x, new_window_pos.y, new_window_size.x, new_window_size.y);

            m_engine_window_pos  = new_window_pos;
            m_engine_window_size = new_window_size;
            SceneManager::Get().SetWindowSize(m_engine_window_size);
        }

        ImGui::End();
    }

    void EditorUI::showEditorInspectorWindow(bool* p_open)
    {
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton  | ImGuiDockNodeFlags_NoCloseButton;
        ImGui::SetNextWindowClass(&window_class);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        if (!ImGui::Begin("Inspector", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        GameObject* selected_object = getSelectedGameObject();
        if (selected_object == nullptr)
        {
            ImGui::End();
            return;
        }
        const std::string& name = selected_object->GetName();
        static char        cname[128];
        memset(cname, 0, 128);
        memcpy(cname, name.c_str(), name.size());

        ImGui::Text("Name");
        ImGui::SameLine();
        ImGui::InputText("##Name", cname, IM_ARRAYSIZE(cname), ImGuiInputTextFlags_ReadOnly);

        static ImGuiTableFlags flags                      = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings;
        // auto&&                 selected_object_components = selected_object->GetComponent();
        // for (auto component_ptr : selected_object_components)
        // {
        //     m_editor_ui_creator["TreeNodePush"](("<" + component_ptr.getTypeName() + ">").c_str(), nullptr);
        //     auto object_instance = Reflection::ReflectionInstance(
        //         Pilot::Reflection::TypeMeta::newMetaFromName(component_ptr.getTypeName().c_str()),
        //         component_ptr.operator->());
        //     createComponentUI(object_instance);
        //     m_editor_ui_creator["TreeNodePop"](("<" + component_ptr.getTypeName() + ">").c_str(), nullptr);
        // }
        ImGui::End();
    }

    GameObject * EditorUI::getSelectedGameObject() const
    {
        return nullptr;
    }
    void EditorUI::processEditorCommand()
    {
        
    }
/*****************************************************************************/
/*                           EditorUI Protected                              */
/*****************************************************************************/
    void EditorUI::registerInput()
    {
        
    }
} // namespace TinyEngine