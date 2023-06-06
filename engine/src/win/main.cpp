#include "Window.h"
#include <iostream>
#include "WindowMsgMap.h"
#include "EngineException.h"
// Main code
int main(int, char**)
{
    try
    {
        Window wnd{1280, 720, L"TinyRenderer"};
        // WindowMsgMap wndMsgMap{};
        MSG msg;
        BOOL result;
        while(Window::GetWindowCount() > 0)
        {
            result = ::GetMessageW(&msg, nullptr, 0, 0);
            // std::cout << "  msg: " << wndMsgMap(msg.message, msg.lParam, msg.wParam) << std::endl;
            if(result == 0)
            {
                auto hwnd = ::GetForegroundWindow();
                Window::CloseWindow(hwnd);
            }
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }

        if (result < 0)
            return -1;
    }
    catch(const EngineException& e)
    {
        MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch(const std::exception& e)
    {
        MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch(...)
    {
        MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    
    
    return 0;
}