#include "Window.h"
#include "input/Input.h"

using namespace Input;
Window::Window(int width, int height, const wchar_t *name)
    : width(width),
      height(height)
{
    Input::InputSystem::RegisterInput(kbd, mouse);

    // register
    auto inputPtr = Input::InputSystem::GetPtr().get();
    registerOnKeyFunc(std::bind(&Input::InputSystem::OnKey, inputPtr, std::placeholders::_1, std::placeholders::_2));
    registerOnCursorPosFunc(std::bind(&Input::InputSystem::OnCursorPos, inputPtr, std::placeholders::_1, std::placeholders::_2));
    registerOnMouseDeltaFunc(std::bind(&Input::InputSystem::OnMouseMoveDelta, inputPtr, std::placeholders::_1, std::placeholders::_2));
}

Window::~Window()
{
}

std::optional<int> Window::ProcessMessages() noexcept
{
    return {};
}


void Window::ClearKeyboard() noexcept
{
    kbd.ClearState();
}
void Window::OnKeyPressed(unsigned char keycode) noexcept { 
    kbd.OnKeyPressed(keycode);
    for(auto& func : m_onKeyFunc)
        func(keycode, static_cast<int>(KeyState::KEY_PRESSED));
}
void Window::OnKeyReleased(unsigned char keycode) noexcept 
{ 
    kbd.OnKeyReleased(keycode);
    for(auto& func : m_onKeyFunc)
        func(keycode, static_cast<int>(KeyState::KEY_RELEASED));
}
void Window::OnChar(unsigned char character) noexcept 
{ 
    kbd.OnChar(character); 
    for(auto& func : m_onCharFunc)
        func(character);
}
void Window::OnMouseMove(int x, int y) noexcept { 
    mouse.OnMouseMove(x, y); 
    for(auto& func : m_onCursorPosFunc)
        func(x, y);
}

void Window::OnMouseMoveDelta(int x, int y) noexcept { 
    mouse.OnMouseMoveDelta(x, y); 
    for(auto& func : m_onMouseMoveDeltaFunc)
        func(x, y);
}
void Window::OnMouseLeave() noexcept { 
    mouse.OnMouseLeave(); 
    for(auto& func : m_onCursorEnterFunc)
        func(0);
    
}
void Window::OnMouseEnter() noexcept { 
    mouse.OnMouseEnter();
    for(auto& func : m_onCursorEnterFunc)
        func(1);
}
void Window::OnMouseLeftPressed() noexcept { 
    mouse.OnLeftPressed();
    for(auto& func : m_onMouseButtonFunc)
        func(static_cast<int>(MouseButton::Left), static_cast<int>(MouseButtonState::PRESSED));
}
void Window::OnMouseLeftReleased() noexcept 
{ 
    mouse.OnLeftReleased();
    for(auto& func : m_onMouseButtonFunc)
        func(static_cast<int>(MouseButton::Left), static_cast<int>(MouseButtonState::RELEASED));
}
void Window::OnMouseRightPressed() noexcept { 
    mouse.OnRightPressed(); 
    for(auto& func : m_onMouseButtonFunc)
        func(static_cast<int>(MouseButton::Right), static_cast<int>(MouseButtonState::PRESSED));
}
void Window::OnMouseRightReleased() noexcept { 
    mouse.OnRightReleased(); 
    for(auto& func : m_onMouseButtonFunc)
        func(static_cast<int>(MouseButton::Right), static_cast<int>(MouseButtonState::RELEASED));
}
void Window::OnMouseWheelPressed() noexcept { 
    mouse.OnWheelPressed(); 
    for(auto& func : m_onMouseButtonFunc)
        func(static_cast<int>(MouseButton::Middle), static_cast<int>(MouseButtonState::PRESSED));
}
void Window::OnMouseWheelReleased() noexcept { 
    mouse.OnWheelReleased(); 
    for(auto& func : m_onMouseButtonFunc)
        func(static_cast<int>(MouseButton::Middle), static_cast<int>(MouseButtonState::RELEASED));
}
void Window::OnMouseWheelUp() noexcept { 
    mouse.OnWheelUp();
}
void Window::OnMouseWheelDown() noexcept { 
    mouse.OnWheelDown();
}
void Window::OnMouseWheelDelta(int delta) noexcept { 
    mouse.OnWheelDelta(delta); 
    for(auto& func : m_onScrollFunc)
        func(0, delta);
}