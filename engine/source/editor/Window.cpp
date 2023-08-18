#include "Window.h"

Window::Window(int width, int height, const wchar_t* name)
    :
    width(width),
    height(height)
{
    
}

Window::~Window()
{
    
}


std::optional<int> Window::ProcessMessages() noexcept
{
    return {};
}