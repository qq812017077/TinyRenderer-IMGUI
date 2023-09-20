#include "Win32Window.h"
#include <iostream>
#include "Exceptions.h"
#include "DirectXGraphics.h"
#include "imgui/backends/imgui_impl_win32.h"
WindowClass WindowClass::wndClass;

// error exception macro
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define LOG(X) std::cout << X << std::endl;
#define BIT_IS_ZERO(val, bit) (((val) & (1 << (bit))) == 0)

Win32Window::Win32Window(int width, int height, const wchar_t * name): Window(width, height, name)
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

    hWnd = ::CreateWindowW(
        WindowClass::GetName(), name, WS_OVERLAPPEDWINDOW, 
        wr.left,  wr.top, width, height, 
        nullptr, nullptr, WindowClass::GetInstance(), this);
    
    if(hWnd == nullptr)
    {
        throw WND_LAST_EXCEPT();
    }

    pGfx = std::make_unique<DirectXGraphics>(hWnd);
    
    //register mouse raw input device
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01; // mouse page
    rid.usUsage = 0x02; // mouse usage
    rid.dwFlags = 0;
    rid.hwndTarget = nullptr;
    if(!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
    {
        throw WND_LAST_EXCEPT();
    }

    ::ShowWindow(hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hWnd);

    LOG("construct Window..  done");
}

Win32Window::~Win32Window()
{
    ::DestroyWindow(hWnd);
}

void Win32Window::CloseWindow() noexcept
{
    LOG("Close window: " << hWnd);
    ::DestroyWindow(hWnd);
}

void Win32Window::ShowMessageBox(const std::string& title, const std::string& message) const noexcept
{
    ::MessageBox(hWnd, title.c_str(), message.c_str(), MB_OK | MB_ICONEXCLAMATION);
}
void Win32Window::SetTitle(const std::wstring& title)
{
    if(!::SetWindowTextW(hWnd, title.c_str()))
    {
        throw WND_LAST_EXCEPT();
    }
}

LRESULT Win32Window::HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if(ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }
    const auto imio = ImGui::GetIO();
    switch (msg)
    {
    case WM_SIZE:
        {
            width = LOWORD(lParam);
            height = HIWORD(lParam);
             pGfx->OnResize(width, height);
            return 0;
        }
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_QUIT:
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    // Window resize
    case WM_SIZING:
        {
            RECT* rect = reinterpret_cast<RECT*>(lParam);
            width = rect->right - rect->left;
            height = rect->bottom - rect->top;
            pGfx->OnResize(width, height);
            return 0;
        }
    // Keyboard part
    case WM_KILLFOCUS:
        ClearKeyboard();
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: // we hope to catch ALT key
        if(imio.WantCaptureKeyboard)
            break;
        if(BIT_IS_ZERO(lParam,30) || kbd.AllowRepeat()) // 30 is zero means, it not repeat.
        {
            OnKeyPressed(static_cast<unsigned char>(wParam));
        }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP: // yeah, we hope to do something like comment above
        if(imio.WantCaptureKeyboard)
            break;
        // LOG("Key Up: " << static_cast<unsigned char>(wParam));
        OnKeyReleased(static_cast<unsigned char>(wParam));
        break;
    case WM_CHAR:
        OnChar(static_cast<unsigned char>(wParam));
        break;
    }
    // std::cout << "imio.WantCaptureKeyboard: " << imio.WantCaptureKeyboard << std::endl;
    // std::cout << "imio.WantCaptureMouse: " << imio.WantCaptureMouse << std::endl;
    
    // Mouse part
    switch (msg)
    {
        case WM_MOUSEMOVE:
            {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam); 
                
                // if mouse is in window, send move event
                if(x >= 0 && x < width && y >= 0 && y < height)
                {
                    OnMouseMove(x, y);
                    if(!mouse.IsInWindow())
                    {
                        SetCapture(hwnd);
                        OnMouseEnter();
                    }
                }else // if mouse is out of window, send leave event
                {
                    //equals to
                    if( wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
                    {
                        OnMouseMove(x, y);
                    }else
                    {
                        ReleaseCapture();
                        OnMouseLeave();
                    }
                }
                break;
            }
        case WM_LBUTTONDOWN:
            {
                OnMouseLeftPressed();
                break;
            }
        case WM_RBUTTONDOWN:
            {
                OnMouseRightPressed();
                break;
            }
        case WM_MBUTTONDOWN:
            {
                OnMouseWheelPressed();
                break;
            }
        case WM_LBUTTONUP:
            {
                if(imio.WantCaptureMouse) break;
                OnMouseLeftReleased();
                break;
            }
        case WM_RBUTTONUP:
            {
                OnMouseRightReleased();
                break;
            }
        case WM_MBUTTONUP:
            {
                OnMouseWheelReleased();
                break;
            }
        case WM_MOUSEWHEEL:
            {
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                OnMouseWheelDelta(delta);
                break;
            }
    }
    
    switch (msg)
    {
    case WM_INPUT:
        UINT rawInputSize;
        if(GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &rawInputSize, sizeof(RAWINPUTHEADER)) == -1)
            break;
        rawInputBuffer.resize(rawInputSize);
        if(GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawInputBuffer.data(), &rawInputSize, sizeof(RAWINPUTHEADER)) != rawInputSize)
            break;
        auto& rawInput = reinterpret_cast<const RAWINPUT&>(*rawInputBuffer.data());
        if(rawInput.header.dwType == RIM_TYPEMOUSE)
        {
            if(rawInput.data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
            {
                OnMouseMoveDelta(rawInput.data.mouse.lLastX, rawInput.data.mouse.lLastY);
            }
        }
        break;
    
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

std::optional<int> Win32Window::ProcessMessages() noexcept
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
    Win32Window* const pWnd = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
}

LRESULT WINAPI WindowClass::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (msg == WM_NCCREATE)
    {
        // extract ptr to window class from creation data
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Win32Window* const pWnd = static_cast<Win32Window*>(pCreate->lpCreateParams);
        // set WinAPI-managed user data to store ptr to window class
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        // set message proc to normal (non-setup) handler now that setup is finished
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&HandleMsgThunk));
        // forward message to window instance handler
        return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}