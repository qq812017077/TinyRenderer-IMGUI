#pragma once
#include "Window.h"
#include "Timer.h"
class App
{
public:
    App();

    int Run();
private:
    void DoFrame();
private:
    // wnd pointer
    std::unique_ptr<Window> pWnd;
    Timer timer;
};

