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
#ifdef _WIN32
    Window wnd;
#endif
    Timer timer;
};

