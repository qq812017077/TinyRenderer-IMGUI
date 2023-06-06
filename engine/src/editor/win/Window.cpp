#include "Window.h"
#include <iostream>

Window::WindowClass Window::WindowClass::wndClass;
std::unordered_map<HWND, Window*> Window::window_map{};

#define LOG(X) std::cout << X << std::endl;
#define BIT_IS_ZERO(val, bit) (((val) & (1 << (bit))) == 0)

Window::Window(int width, int height, const wchar_t * name): kbd()
{
    LOG("construct Window..");
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;
    if (!AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE))
    {
        throw WND_LAST_EXCEPT();
    }
    this->width = width;
    this->height = height;

    hWnd = ::CreateWindowW(
        WindowClass::GetName(), name, WS_OVERLAPPEDWINDOW, 
        wr.left,  wr.top, width, height, 
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

void Window::SetTitle(const std::wstring& title) noexcept
{
    if(!::SetWindowTextW(hWnd, title.c_str()))
    {
        throw WND_LAST_EXCEPT();
    }
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

    // Keyboard part
    case WM_KILLFOCUS:
        kbd.ClearState();
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: // we hope to catch ALT key
        if(BIT_IS_ZERO(lParam,30) || kbd.AllowRepeat()) // 30 is zero means, it not repeat.
        {
            kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
        }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP: // yeah, we hope to do something like comment above
        // LOG("Key Up: " << static_cast<unsigned char>(wParam));
        kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
        break;
    case WM_CHAR:
        kbd.OnChar(static_cast<unsigned char>(wParam));
        break;
    // Mouse part
    //event of mouse move out of window
    case WM_MOUSEMOVE:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            // if mouse is in window, send move event
            if(x >= 0 && x < width && y >= 0 && y < height)
            {
                mouse.OnMouseMove(x, y);
                if(!mouse.IsInWindow())
                {
                    SetCapture(hwnd);
                    mouse.OnMouseEnter();
                }
            }else // if mouse is out of window, send leave event
            {
                // if(mouse.LeftPressed() || mouse.RightPressed() || mouse.WheelPressed())
                //equals to
                if( wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
                {
                    mouse.OnMouseMove(x, y);
                }else
                {
                    ReleaseCapture();
                    mouse.OnMouseLeave();
                }
            }
            break;
        }
    case WM_LBUTTONDOWN:
        {
            mouse.OnLeftPressed();
            break;
        }
    case WM_RBUTTONDOWN:
        {
            mouse.OnRightPressed();
            break;
        }
    case WM_MBUTTONDOWN:
        {
            mouse.OnWheelPressed();
            break;
        }
    case WM_LBUTTONUP:
        {
            mouse.OnLeftReleased();
            break;
        }
    case WM_RBUTTONUP:
        {
            mouse.OnRightReleased();
            break;
        }
    case WM_MBUTTONUP:
        {
            mouse.OnWheelReleased();
            break;
        }
    case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            mouse.OnWheelDelta(delta);
            break;
        }
    }
    
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Window Class part
std::optional<int> Window::ProcessMessages() noexcept
{
    MSG msg;
    while( PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if(msg.message == WM_QUIT)
        {
            return msg.wParam;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return {};
}

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