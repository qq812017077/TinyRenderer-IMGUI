#include "EditorUI.h"
#include "ui/GUI.h"
#include "Window.h"
#include "world/WorldManager.h"
#include "object/GameObject.h"
#include "scene/SceneManager.h"
namespace TinyEngine
{
    
    std::vector<std::pair<std::string, bool>>   g_editor_node_state_array;
    int                                         g_node_depth = -1;
    bool                                        g_mipmap_mode = false;
    bool                                        g_aniso_mode = true;
    EditorUI::EditorUI(Editor* editor)
    {
        m_editor = editor;

        m_editor_ui_creator["TreeNodePush"] = [this](std::string name, void* value_ptr) -> void {
            static ImGuiTableFlags flags      = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings;
            bool                   node_state = false;
            g_node_depth++;
            auto pGo = static_cast<GameObject*>(value_ptr);
            ImGuiTreeNodeFlags flag = pGo->transform().GetChildCount() > 0 ? ImGuiTreeNodeFlags_OpenOnArrow : ImGuiTreeNodeFlags_Leaf;
            bool isSelected = g_editor_global_context.m_scene_manager->getSelectedObjectID() == pGo->GetInstanceID();
            flag |= isSelected ? ImGuiTreeNodeFlags_Selected : 0;
            if (g_node_depth > 0)
            {
                if (g_editor_node_state_array[g_node_depth - 1].second)
                {
                    node_state = ImGui::TreeNodeEx(name.c_str(), flag);
                    // if tree node is clicked, then select the game object
                }
                else
                {
                    g_editor_node_state_array.emplace_back(std::pair(name.c_str(), node_state));
                    return;
                }
            }
            else
            {
                node_state = ImGui::TreeNodeEx(name.c_str(), flag);
            }
            
            if (!isSelected && ImGui::IsItemClicked(0)) 
                g_editor_global_context.m_scene_manager->onGameObjectSelected(pGo->GetInstanceID());
            
            g_editor_node_state_array.emplace_back(std::pair(name.c_str(), node_state));
        };

        m_editor_ui_creator["TreeNodePop"] = [this](std::string name, void* value_ptr) -> void {
            if (g_editor_node_state_array[g_node_depth].second)
            {
                ImGui::TreePop();
            }
            g_editor_node_state_array.pop_back();
            g_node_depth--;
        };

        m_editor_ui_creator["Transform"] = [this](std::string name, void* value_ptr) -> void {
            // draw line
            ImGui::Separator();
            ImGui::Text(name.c_str());
            Transform* trans_ptr = static_cast<Transform*>(value_ptr);
            auto localPos = trans_ptr->GetLocalPosition();
            auto localEuler = trans_ptr->GetLocalEulerAngle();
            auto scale = trans_ptr->GetScale();
            float position_val[3] = {localPos.x, localPos.y, localPos.z};
            float rotation_val[3] = {localEuler.x, localEuler.y, localEuler.z};
            float scale_val[3] = {scale.x, scale.y, scale.z};
            ImGui::TableNextColumn();
            ImGui::DragFloat3("Position", position_val, 0.1f);
            ImGui::TableNextColumn();
            ImGui::DragFloat3("Rotation", rotation_val, 0.1f);
            ImGui::TableNextColumn();
            ImGui::DragFloat3("Scale", scale_val, 0.1f);
            
            trans_ptr->SetLocalPosition(Vector3(position_val[0], position_val[1], position_val[2]));
            trans_ptr->SetLocalEulerAngle(Vector3(rotation_val[0], rotation_val[1], rotation_val[2]));
            trans_ptr->SetScale(Vector3(scale_val[0], scale_val[1], scale_val[2]));
            // drawSelectedEntityAxis();
        };

        m_editor_ui_creator["int"] = [this](std::string name, void* value_ptr) -> void {
            if (g_node_depth == -1)
            {
                std::string label = "##" + name;
                ImGui::Text(name.c_str());
                ImGui::SameLine();
                ImGui::InputInt(label.c_str(), static_cast<int*>(value_ptr));
            }
            else
            {
                if (g_editor_node_state_array[g_node_depth].second)
                {
                    std::string full_label = "##" + getLeafUINodeParentLabel() + name;
                    ImGui::Text((name + ":").c_str());
                    ImGui::InputInt(full_label.c_str(), static_cast<int*>(value_ptr));
                }
            }
        };
        m_editor_ui_creator["float"] = [this](std::string name, void* value_ptr) -> void {
            if (g_node_depth == -1)
            {
                std::string label = "##" + name;
                ImGui::Text(name.c_str());
                ImGui::SameLine();
                ImGui::InputFloat(label.c_str(), static_cast<float*>(value_ptr));
            }
            else
            {
                if (g_editor_node_state_array[g_node_depth].second)
                {
                    std::string full_label = "##" + getLeafUINodeParentLabel() + name;
                    ImGui::Text((name + ":").c_str());
                    ImGui::InputFloat(full_label.c_str(), static_cast<float*>(value_ptr));
                }
            }
        };
        m_editor_ui_creator["Vector3"] = [this](std::string name, void* value_ptr) -> void {
            Vector3* vec_ptr = static_cast<Vector3*>(value_ptr);
            float    val[3]  = {vec_ptr->x, vec_ptr->y, vec_ptr->z};
            if (g_node_depth == -1)
            {
                std::string label = "##" + name;
                ImGui::Text(name.c_str());
                ImGui::SameLine();
                ImGui::DragFloat3(label.c_str(), val);
            }
            else
            {
                if (g_editor_node_state_array[g_node_depth].second)
                {
                    std::string full_label = "##" + getLeafUINodeParentLabel() + name;
                    ImGui::Text((name + ":").c_str());
                    ImGui::DragFloat3(full_label.c_str(), val);
                }
            }
            vec_ptr->x = val[0];
            vec_ptr->y = val[1];
            vec_ptr->z = val[2];
        };

        m_editor_ui_creator["Quaternion"] = [this](std::string name, void* value_ptr) -> void {
            Quaternion* qua_ptr = static_cast<Quaternion*>(value_ptr);
            float       val[4]  = {qua_ptr->x, qua_ptr->y, qua_ptr->z, qua_ptr->w};
            if (g_node_depth == -1)
            {
                std::string label = "##" + name;
                ImGui::Text(name.c_str());
                ImGui::SameLine();
                ImGui::DragFloat4(label.c_str(), val);
            }
            else
            {
                if (g_editor_node_state_array[g_node_depth].second)
                {
                    std::string full_label = "##" + getLeafUINodeParentLabel() + name;
                    ImGui::Text((name + ":").c_str());
                    ImGui::DragFloat4(full_label.c_str(), val);
                }
            }
            qua_ptr->x = val[0];
            qua_ptr->y = val[1];
            qua_ptr->z = val[2];
            qua_ptr->w = val[3];
        };
        // m_editor_ui_creator["std::string"] = [this, &path_service, &asset_folder](std::string name,
        //                                                                           void*       value_ptr) -> void {
        //     if (g_node_depth == -1)
        //     {
        //         std::string label = "##" + name;
        //         ImGui::Text(name.c_str());
        //         ImGui::SameLine();
        //         ImGui::Text((*static_cast<std::string*>(value_ptr)).c_str());
        //     }
        //     else
        //     {
        //         if (g_editor_node_state_array[g_node_depth].second)
        //         {
        //             std::string full_label = "##" + getLeafUINodeParentLabel() + name;
        //             ImGui::Text((name + ":").c_str());
        //             std::string value_str = *static_cast<std::string*>(value_ptr);
        //             if (value_str.find_first_of('/') != std::string::npos)
        //             {
        //                 std::filesystem::path value_path(value_str);
        //                 if (value_path.is_absolute())
        //                 {
        //                     value_path = path_service.getRelativePath(asset_folder, value_path);
        //                 }
        //                 value_str = value_path.generic_string();
        //                 if (value_str.size() >= 2 && value_str[0] == '.' && value_str[1] == '.')
        //                 {
        //                     value_str.clear();
        //                 }
        //             }
        //             ImGui::Text(value_str.c_str());
        //         }
        //     }
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
        if(ImGui::Begin("FPS"))
        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
        ImGui::End();
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
            if (ImGui::BeginMenu("Setting"))
            {
                
            }
            ImGui::EndMenuBar(); 
        }
        ImGui::End();
    }

    void EditorUI::showEditorHierarchysWindow(bool* p_open)
    {
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton  | ImGuiDockNodeFlags_NoCloseButton;
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
            if (pGO->transform().GetChildCount() > 0) buildHierarchysUITree(pGO);
            else if(pGO->transform().GetParent() == nullptr)
            {
                ImGui::Indent(10.f);
                if (ImGui::Selectable(pGO->GetName().c_str(), g_editor_global_context.m_scene_manager->getSelectedObjectID() == object_id))
                {
                    g_editor_global_context.m_scene_manager->onGameObjectSelected(object_id);
                    break;
                }
                ImGui::Unindent(10.f);
            }
        }
        // check if this window is clicked
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
        {
            g_editor_global_context.m_scene_manager->onGameObjectSelected(INVALID_GO_ID);
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

        
        // check if this window is clicked
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
        {
            g_editor_global_context.m_scene_manager->onGameObjectSelected(INVALID_GO_ID);
        }
        ImGui::End();
    }

    void EditorUI::showEditorGameWindow(bool* p_open)
    {
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        ImGui::SetNextWindowClass(&window_class);
        ImGuiIO& io = ImGui::GetIO();
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
        switch (g_editor_global_context.m_scene_manager->getEditorAxisMode())
        {
            case EditorAxisMode::TranslateMode:
                trans_button_ckecked  = true;
                rotate_button_ckecked = false;
                scale_button_ckecked  = false;
                break;
            case EditorAxisMode::RotateMode:
                trans_button_ckecked  = false;
                rotate_button_ckecked = true;
                scale_button_ckecked  = false;
                break;
            case EditorAxisMode::ScaleMode:
                trans_button_ckecked  = false;
                rotate_button_ckecked = false;
                scale_button_ckecked  = true;
                break;
            default:
                break;
        }

        if (ImGui::BeginMenuBar())
        {
            ImGui::Indent(10.f);
            drawAxisToggleButton("Trans", trans_button_ckecked, EditorAxisMode::TranslateMode);
            ImGui::Unindent();

            ImGui::SameLine();

            drawAxisToggleButton("Rotate", rotate_button_ckecked, EditorAxisMode::RotateMode);

            ImGui::SameLine();

            drawAxisToggleButton("Scale", scale_button_ckecked, EditorAxisMode::ScaleMode);

            ImGui::SameLine();

            float indent_val = 0.0f;
            indent_val       = g_editor_global_context.m_input_manager->getEngineWindowSize().x - 100.0f;
            ImGui::Indent(indent_val);
            // if (m_is_editor_mode)
            // {
            //     ImGui::PushID("Editor Mode");
            //     ImGui::Button("Editor Mode");
            //     if (ImGui::IsItemClicked(0))
            //     {
            //         m_is_editor_mode = !m_is_editor_mode;
            //         drawSelectedEntityAxis();
            //         // g_is_editor_mode = false;
            //         // setFocusMode(true);
            //     }
            //     ImGui::PopID();
            // }
            // else
            // {
            //     ImGui::Button("Game Mode");
            //     if (ImGui::IsItemClicked(0))
            //     {
            //         m_is_editor_mode = !m_is_editor_mode;
            //         // g_is_editor_mode = true;
            //         // SceneManager::getInstance().setMainViewMatrix(m_tmp_uistate->m_editor_camera->getViewMatrix());
            //     }
            // }
            // setEditorMode(m_is_editor_mode);
            ImGui::Unindent();
            ImGui::EndMenuBar();
        }

        if (!g_editor_global_context.m_is_editor_mode)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Press Left Alt key to display the mouse cursor!");
        }

        Vector2 new_window_pos  = {0.0f, 0.0f};
        Vector2 new_window_size = {0.0f, 0.0f};
        
        // 获取当前dockWindow的pos和size
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        ImVec2 content = ImGui::GetContentRegionAvail();
        auto menu_bar_rect = ImGui::GetCurrentWindow()->MenuBarRect();
        
        new_window_pos.x        = pos.x;
        new_window_pos.y        = menu_bar_rect.Max.y; // pos.y + 38.0f; 
        new_window_size.x       = size.x;
        new_window_size.y       = (size.y + pos.y) - menu_bar_rect.Max.y; // size.y - 38.0f;

        
        // 在dockWindow内部显示pos和size
        // ImGui::Text("Pos: (%.2f, %.2f)", pos.x, pos.y);
        // ImGui::Text("Size: (%.2f, %.2f)", size.x, size.y);
        // ImGui::Text("Content: (%.2f, %.2f)", content.x, content.y);

        // if (new_window_pos != m_engine_window_pos || new_window_size != m_engine_window_size)
        {

            g_editor_global_context.m_graphics->UpdateRenderSceneViewPort(new_window_pos.x, new_window_pos.y,
                                            new_window_size.x, new_window_size.y);
            g_editor_global_context.m_input_manager->setEngineWindowPos(new_window_pos);
            g_editor_global_context.m_input_manager->setEngineWindowSize(new_window_size);
        }

        const Level* current_active_level = WorldManager::Get().GetCurrentActiveLevel();
        if (current_active_level != nullptr)
        {
            const auto& all_gobjects = current_active_level->GetAllGameObjects();
            for (auto& id_object_pair : all_gobjects)
            {
                const size_t      object_id = id_object_pair.first;
                GameObject*        pGO    = id_object_pair.second;
                pGO->OnGUI();
            }
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

        GameObject* selected_object = g_editor_global_context.m_scene_manager->getSelectedGameObject();
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
        auto pTrans = selected_object->GetComponent<Transform>();
        m_editor_ui_creator["Transform"]("<Transform>", pTrans);
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

    
    void EditorUI::buildHierarchysUITree(GameObject* node)
    {
        m_editor_ui_creator["TreeNodePush"](node->GetName().c_str(), node);
        for(int i = 0, imax = node->transform().GetChildCount(); i < imax; ++i)
        {
            auto pChild = node->transform().GetChild(i);
            buildHierarchysUITree(pChild->GetGameObject());
        }
        m_editor_ui_creator["TreeNodePop"](node->GetName().c_str(), node);
    }

    std::string EditorUI::getLeafUINodeParentLabel()
    {
        std::string parent_label;
        size_t         array_size = g_editor_node_state_array.size();
        for (size_t index = 0; index < array_size; index++)
        {
            parent_label += g_editor_node_state_array[index].first + "::";
        }
        return parent_label;
    }


    void EditorUI::drawAxisToggleButton(const char* string_id, bool check_state, EditorAxisMode axis_mode)
    {
        if (check_state)
        {
            ImGui::PushID(string_id);
            ImVec4 check_button_color = ImVec4(93.0f / 255.0f, 10.0f / 255.0f, 66.0f / 255.0f, 1.00f);
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(check_button_color.x, check_button_color.y, check_button_color.z, 0.40f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, check_button_color);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, check_button_color);
            ImGui::Button(string_id);
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }
        else
        {
            if (ImGui::Button(string_id))
            {
                check_state = true;
                g_editor_global_context.m_scene_manager->setEditorAxisMode((EditorAxisMode)axis_mode);
                g_editor_global_context.m_scene_manager->drawSelectedEntityAxis();
            }
        }
    }

/*****************************************************************************/
/*                           EditorUI Protected                              */
/*****************************************************************************/

} // namespace TinyEngine