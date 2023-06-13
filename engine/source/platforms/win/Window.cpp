#include "Window.h"
#include "WindowClass.h"
#include <iostream>
#include "EngineWin.h"
#include "Exceptions.h"

WindowClass WindowClass::wndClass;

// error exception macro

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
    windowInfo.width = width;
    windowInfo.height = height;

    windowInfo.hWnd = ::CreateWindowW(
        WindowClass::GetName(), name, WS_OVERLAPPEDWINDOW, 
        wr.left,  wr.top, width, height, 
        nullptr, nullptr, WindowClass::GetInstance(), this);
    
    if(windowInfo.hWnd == nullptr)
    {
        throw WND_LAST_EXCEPT();
    }

    pGfx = std::make_unique<Graphics>(this->windowInfo);
    
    ::ShowWindow(windowInfo.hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(windowInfo.hWnd);
    
    LOG("construct Window..  done");
}

Window::~Window()
{
    ::DestroyWindow(windowInfo.hWnd);
}

void Window::CloseWindow() noexcept
{
    LOG("Close window: " << windowInfo.hWnd);
    ::DestroyWindow(windowInfo.hWnd);
}

void Window::SetTitle(const std::wstring& title)
{
    if(!::SetWindowTextW(windowInfo.hWnd, title.c_str()))
    {
        throw WND_LAST_EXCEPT();
    }
}

LRESULT Window::HandleMsg(WinMsg message) noexcept
{
    // unzip message
    HWND hwnd = message.hwnd;
    UINT msg = message.message;
    WPARAM wParam = message.wParam;
    LPARAM lParam = message.lParam;

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
            if(x >= 0 && x < windowInfo.width && y >= 0 && y < windowInfo.height)
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
            // convert WPARAM to int
            return static_cast<int>(msg.wParam);
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return {};
}



