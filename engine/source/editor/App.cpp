#include "App.h"
#include <iomanip>
#ifdef _WIN32
#include "Win32Window.h"
#elif __linux__
#endif
App::App()
{
// if win32 , use win32 window
#ifdef _WIN32
    pWnd = std::make_unique<Win32Window>(1280, 720, L"TinyRenderer");
#elif __linux__
    pWnd = std::make_unique<LinuxWindow>(1280, 720, L"TinyRenderer");
#endif
}

int App::Run()
{
    while (true)
    {
        if (const auto ecode = Window::ProcessMessages())
        {
            return *ecode;
        }
        DoFrame();
    }
    return -1;
}

void App::DoFrame()
{
    const float c = sin(timer.Peek()) / 2.0f + 0.5f;
    pWnd->Gfx()->ClearBuffer(c, c, 1.0f);
    pWnd->Gfx()->EndFrame();
    // const float t = timer.Peek();
    // //print
    // std::cout << "Time elapsed" << std::setprecision(1) << std::fixed << t  << std::endl;
    // while(!wnd.GetMouse().IsEmpty())
    // {
    //     const auto e = wnd.ReadMouseEvent();
    //     if(e.IsValid())
    //     {
    //         // switch(e.GetState())
    //         // {
    //         //     case MouseState::LEAVE:
    //         //         // gone
    //         //         std::cout << "Gone" << std::endl;
    //         //         break;
    //         //     case MouseState::ENTER:
    //         //         // back
    //         //         std::cout << "Back" << std::endl;
    //         //         break;
    //         //     case MouseState::MOVE:
    //         //         // print mouse position
    //         //         std::cout << "Mouse Move: " << e.GetPosX() << ", " << e.GetPosY() << std::endl;
    //         //         break;
    //         //     case MouseState::WHEEL_UP:
    //         //         // print mouse position
    //         //         std::cout << "Mouse Wheel Up: " << e.GetPosX() << ", " << e.GetPosY() << std::endl;
    //         //         break;
    //         //     case MouseState::WHEEL_DOWN:
    //         //         // print mouse position
    //         //         std::cout << "Mouse Wheel Down: " << e.GetPosX() << ", " << e.GetPosY() << std::endl;
    //         //         break;
    //         // }
    //     }
    // }
}
