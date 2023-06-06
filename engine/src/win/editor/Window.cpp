#include "Window.h"
#include <iostream>

Window::WindowClass Window::WindowClass::wndClass;
std::unordered_map<HWND, Window*> Window::window_map{};

#define LOG(X) std::cout << X << std::endl;

Window::Window(int width, int height, const wchar_t * name)
{
    LOG("construct Window..");
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;
    if FAILED(AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE))
    {
        throw WND_LAST_EXCEPT();
    }

    hWnd = ::CreateWindowW(
        WindowClass::GetName(), name, WS_OVERLAPPEDWINDOW, 
        wr.left,  wr.top, wr.right - wr.left, wr.bottom - wr.top, 
        nullptr, nullptr, WindowClass::GetInstance(), this);
    
    // log hWnd
    // LOG("hWnd: " << hWnd);
    if(hWnd == nullptr)
    {
        throw WND_LAST_EXCEPT();
    }
    ::ShowWindow(hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hWnd);

    window_map[hWnd] = this;

    LOG("construct Window..  done");
}

Window::~Window()
{
    window_map.erase(hWnd);
    ::DestroyWindow(hWnd);
}

LRESULT Window::HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (msg)
    {
    case WM_SIZE:
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_QUIT:
    case WM_CLOSE:
        PostQuitMessage(64);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Window Class part
void Window::CloseWindow(HWND &hwnd)
{
    LOG("Try to close window: " << hwnd);
    auto it = window_map.find(hwnd);
    if(it != window_map.end())
    {
        it->second->ClostWindow();
        window_map.erase(it);
    }else
    {
        LOG("CloseWindow: can't find hwnd: " << hwnd);
    }
}
Window::WindowClass::WindowClass() noexcept
    :
    hInst(GetModuleHandle(nullptr))
{
    
    LOG("construct WindowClass..");
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, HandleMsgSetup, 0L, 0L, GetInstance(), nullptr, nullptr, nullptr, nullptr, wndClassName, nullptr };
    RegisterClassExW(&wc);
    LOG("construct WindowClass.. Done");
}

Window::WindowClass::~WindowClass()
{
    UnregisterClassW(wndClassName, GetInstance());
}




// Static methods part
const wchar_t* Window::WindowClass::GetName() noexcept
{
    return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return wndClass.hInst;
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (msg == WM_NCCREATE)
    {
        // extract ptr to window class from creation data
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
        // set WinAPI-managed user data to store ptr to window class
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        // set message proc to normal (non-setup) handler now that setup is finished
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
        // forward message to window instance handler
        return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}