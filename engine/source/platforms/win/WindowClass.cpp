#include <iostream>
#include "WindowClass.h"


#define LOG(X) std::cout << X << std::endl;

WindowClass::WindowClass() noexcept
    :
    hInst(GetModuleHandle(nullptr))
{
    
    LOG("construct WindowClass..");
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, HandleMsgSetup, 0L, 0L, GetInstance(), nullptr, nullptr, nullptr, nullptr, wndClassName, nullptr };
    RegisterClassExW(&wc);
    LOG("construct WindowClass.. Done");
}

WindowClass::~WindowClass()
{
    UnregisterClassW(wndClassName, GetInstance());
}

// Static methods part
const wchar_t* WindowClass::GetName() noexcept
{
    return wndClassName;
}

HINSTANCE WindowClass::GetInstance() noexcept
{
    return wndClass.hInst;
}

LRESULT WINAPI WindowClass::HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    return pWnd->HandleMsg({hwnd, msg, wParam, lParam});
}

LRESULT WINAPI WindowClass::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (msg == WM_NCCREATE)
    {
        // extract ptr to window class from creation data
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
        // set WinAPI-managed user data to store ptr to window class
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        // set message proc to normal (non-setup) handler now that setup is finished
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&HandleMsgThunk));
        // forward message to window instance handler
        return pWnd->HandleMsg({hwnd, msg, wParam, lParam});
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}