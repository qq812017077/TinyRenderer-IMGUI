#include "EditorUI.h"


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
        m_editor->pWnd->Update();
        if (const auto ecode = m_editor->pWnd->ProcessMessages()) return false;

        draw_ui();
        return true;
    }

    void EditorUI::clear()
    {
        
    }

    void EditorUI::draw_ui()
    {
        if( m_editor->imgui.IsEnabled())
        {
            m_editor->imgui.NewFrame();
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
    }
} // namespace TinyEngine