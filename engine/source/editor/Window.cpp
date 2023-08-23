#include "Window.h"
#include "input/Input.h"
Window::Window(int width, int height, const wchar_t *name)
    : width(width),
      height(height)
{
    Input::InputSystem::RegisterInput(kbd, mouse);
}

Window::~Window()
{
}

std::optional<int> Window::ProcessMessages() noexcept
{
    return {};
}

void Window::Update()
{
    kbd.Update();
    mouse.Update();
}

void Window::ClearKeyboard() noexcept
{
    kbd.ClearState();
}
void Window::OnKeyPressed(unsigned char keycode) noexcept { 
    kbd.OnKeyPressed(keycode); 
}
void Window::OnKeyReleased(unsigned char keycode) noexcept 
{ 
    kbd.OnKeyReleased(keycode); 
}
void Window::OnChar(unsigned char character) noexcept 
{ 
    kbd.OnChar(character); 
}
void Window::OnMouseMove(int x, int y) noexcept { 
    mouse.OnMouseMove(x, y); 
}
void Window::OnMouseLeave() noexcept { 
    mouse.OnMouseLeave(); 
}
void Window::OnMouseEnter() noexcept { 
    mouse.OnMouseEnter(); 
}
void Window::OnMouseLeftPressed() noexcept { 
    mouse.OnLeftPressed(); 
}
void Window::OnMouseLeftReleased() noexcept { mouse.OnLeftReleased(); }
void Window::OnMouseRightPressed() noexcept { mouse.OnRightPressed(); }
void Window::OnMouseRightReleased() noexcept { mouse.OnRightReleased(); }
void Window::OnMouseWheelPressed() noexcept { mouse.OnWheelPressed(); }
void Window::OnMouseWheelReleased() noexcept { mouse.OnWheelReleased(); }
void Window::OnMouseWheelUp() noexcept { mouse.OnWheelUp(); }
void Window::OnMouseWheelDown() noexcept { mouse.OnWheelDown(); }
void Window::OnMouseWheelDelta(int delta) noexcept { mouse.OnWheelDelta(delta); }