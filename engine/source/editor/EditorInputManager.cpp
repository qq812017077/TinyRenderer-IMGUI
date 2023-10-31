
#include <iostream>
#include "EditorInputManager.h"
#include "EditorGlobalContext.h"
#include "Window.h"
#include "input/Mouse.h"
#include "world/WorldManager.h"


namespace TinyEngine
{
    void EditorInputManager::initialize()
    {
        registerInput();
    }

    void EditorInputManager::tick(float delta_time)
    {
        processEditorCommand();
    }

    void EditorInputManager::registerInput()
    {
        auto window = g_editor_global_context.m_window_system;

        window->registerOnResetFunc(std::bind(&EditorInputManager::onReset, this));

        // mouse event
        window->registerOnCursorPosFunc(
            std::bind(&EditorInputManager::onCursorPos, this, std::placeholders::_1, std::placeholders::_2));
        window->registerOnCursorEnterFunc(
            std::bind(&EditorInputManager::onCursorEnter, this, std::placeholders::_1));
        window->registerOnScrollFunc(
            std::bind(&EditorInputManager::onScroll, this, std::placeholders::_1, std::placeholders::_2));
        window->registerOnMouseButtonFunc(
            std::bind(&EditorInputManager::onMouseButtonClicked, this, std::placeholders::_1, std::placeholders::_2));
        window->registerOnWindowCloseFunc(
            std::bind(&EditorInputManager::onWindowClosed, this));
        window->registerOnKeyFunc(
            std::bind(&EditorInputManager::onKey, this, std::placeholders::_1, std::placeholders::_2));
}

    void EditorInputManager::processEditorCommand()
    {
        // float           camera_speed  = m_camera_speed;
        // std::shared_ptr editor_camera = g_editor_global_context.m_scene_manager->getEditorCamera();
        // Quaternion      camera_rotate = editor_camera->rotation().inverse();
        // Vector3         camera_relative_pos(0, 0, 0);

    }

    void EditorInputManager::onReset()
    {
        // to do
    }

    void EditorInputManager::onCursorPos(double xpos, double ypos)
    {
        // std::cout << "xpos: " << xpos << " ypos: " << ypos << std::endl;
        if (!g_editor_global_context.m_is_editor_mode)
            return;
        m_mouse_x = static_cast<float>(xpos);
        m_mouse_y = static_cast<float>(ypos);
        if (xpos >= 0.0f && ypos >= 0.0f)
        {
            // if right button is pressed
            // if (Input::InputSystem::Is(Input::MouseButton::right))
            // {
            //     //windows: disable the cursor

            //     // TODO: the camera rotation
                
            // }else 
            // {
            //     //enable the cursor
            //     // TODO:
            //     if (Input::InputSystem::Is(Input::MouseButton::left))
            //     {
            //         // move entity

            //         //
            //     }else
            //     {
            //         // select entity
            //         if (isCursorInRect(m_engine_window_pos, m_engine_window_size))
            //         {
            //             Vector2 cursor_uv = Vector2((m_mouse_x - m_engine_window_pos.x) / m_engine_window_size.x,
            //                                     (m_mouse_y - m_engine_window_pos.y) / m_engine_window_size.y);
            //             updateCursorOnAxis(cursor_uv);
            //         }
            //     }
            // }
        }
        
    }

    void EditorInputManager::onCursorEnter(int entered)
    {
        // std::cout << "entered: " << entered << std::endl;
    }

    void EditorInputManager::onScroll(double xoffset, double yoffset)
    {
        // std::cout << "xoffset: " << xoffset << " yoffset: " << yoffset << std::endl;
    }

    void EditorInputManager::onMouseButtonClicked(int key, int action)
    {
        if (!g_editor_global_context.m_is_editor_mode) return;
        if (m_cursor_on_axis != 3) return;
        // std::cout << "key: " << key << " action: " << action << std::endl;
        Input::MouseButton mouse_button = static_cast<Input::MouseButton>(key);
        Input::MouseButtonState  mouse_state  = static_cast<Input::MouseButtonState>(action);

        auto current_active_level = WorldManager::Get().GetCurrentActiveLevel();
        if (current_active_level == nullptr)
            return;
        if (isCursorInRect(m_engine_window_pos, m_engine_window_size))
        {
            if(mouse_button == Input::MouseButton::Left && mouse_state == Input::MouseButtonState::PRESSED)
            {
                Vector2 picked_uv((m_mouse_x - m_engine_window_pos.x) / m_engine_window_size.x,
                                  (m_mouse_y - m_engine_window_pos.y) / m_engine_window_size.y);
                
                std::cout << "picked_uv: " << picked_uv.x << " " << picked_uv.y << std::endl;
                size_t  go_id = g_editor_global_context.m_scene_manager->PickGuidOfGameObject(picked_uv);
                g_editor_global_context.m_scene_manager->onGameObjectSelected(go_id);
            }
        }
    }

    void EditorInputManager::onWindowClosed()
    {
        // std::cout << "window closed" << std::endl;
    }

    void EditorInputManager::onKey(int key, int scancode)
    {
        // std::cout << "key: " << key << " scancode: " << scancode << std::endl;

    }


    bool EditorInputManager::isCursorInRect(Vector2 pos, Vector2 size) const
    {
        return pos.x <= m_mouse_x && m_mouse_x <= pos.x + size.x && pos.y <= m_mouse_y && m_mouse_y <= pos.y + size.y;
    }

}