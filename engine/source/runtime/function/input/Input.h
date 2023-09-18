#pragma once
#include "core\Singleton.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace Input
{
    enum class EditorCommand : unsigned int
    {
        camera_left      = 1 << 0,  // A
        camera_back      = 1 << 1,  // S
        camera_foward    = 1 << 2,  // W
        camera_right     = 1 << 3,  // D
        camera_up        = 1 << 4,  // Q
        camera_down      = 1 << 5,  // E
        translation_mode = 1 << 6,  // T
        rotation_mode    = 1 << 7,  // R
        scale_mode       = 1 << 8,  // C
        exit             = 1 << 9,  // Esc
        delete_object    = 1 << 10, // Delete
    };

    enum class GameCommand : unsigned int
    {
        forward  = 1 << 0,                 // W
        backward = 1 << 1,                 // S
        left     = 1 << 2,                 // A
        right    = 1 << 3,                 // D
        jump     = 1 << 4,                 // not implemented yet
        squat    = 1 << 5,                 // not implemented yet
        sprint   = 1 << 6,                 // LEFT SHIFT
        fire     = 1 << 7,                 // not implemented yet
        invalid  = (unsigned int)(1 << 31) // lost focus
    };

    class InputSystem : public Singleton<InputSystem>
    {   
        public:
            void Initialize()
            {
                
            }
            void Tick()
            {
                m_keyboard->Tick();
                m_mouse->Tick();
            }

            void OnKey(int key, int action)
            {
                // m_keyboard->OnKey(key, action);
            }
            void OnCursorPos(double current_cursor_x, double current_cursor_y)
            {
                // m_mouse->OnCursorPos(current_cursor_x, current_cursor_y);
            }

        public: // static interface
            static void RegisterInput(Keyboard& keyboard, Mouse& mouse)
            {
                auto& inputSystem = InputSystem::Get();
                inputSystem.m_keyboard = &keyboard;
                inputSystem.m_mouse = &mouse;
            }

            static bool GetKey(KeyCode keycode) noexcept
            {
                return InputSystem::Get().m_keyboard->GetKey(keycode);
            }

            static bool GetKeyDown(KeyCode keycode) noexcept
            {
                return InputSystem::Get().m_keyboard->GetKeyDown(keycode);
            }

            static bool GetKeyUp(KeyCode keycode) noexcept
            {
                return InputSystem::Get().m_keyboard->GetKeyUp(keycode);
            }

            static bool GetMouseButton(Mouse::MouseButton button) noexcept
            {
                return InputSystem::Get().m_mouse->GetMouseButton(button);
            }

            static float GetMouseAxisX() noexcept
            {
                return InputSystem::Get().m_mouse->GetMouseAxisX();
            }

            static float GetMouseAxisY() noexcept
            {
                return InputSystem::Get().m_mouse->GetMouseAxisY();
            }

            static bool GetMouseButtonDown(Mouse::MouseButton button) noexcept
            {
                return InputSystem::Get().m_mouse->GetMouseButtonDown(button);
            }

            static bool GetMouseButtonUp(Mouse::MouseButton button) noexcept
            {
                return InputSystem::Get().m_mouse->GetMouseButtonUp(button);
            }

            static bool GetMouseButton(unsigned char button) noexcept
            {
                return InputSystem::Get().m_mouse->GetMouseButton(static_cast<Mouse::MouseButton>(button));
            }
            
            static bool GetMouseButtonDown(unsigned char button) noexcept
            {
                return InputSystem::Get().m_mouse->GetMouseButtonDown(static_cast<Mouse::MouseButton>(button));
            }

            static bool GetMouseButtonUp(unsigned char button) noexcept
            {
                return InputSystem::Get().m_mouse->GetMouseButtonUp(static_cast<Mouse::MouseButton>(button));
            }

            static float GetMouseScrollWheel() noexcept
            {
                return InputSystem::Get().m_mouse->GetWhellScroll();
            }
        
        private:
            Keyboard* m_keyboard;
            Mouse* m_mouse;
    };
}