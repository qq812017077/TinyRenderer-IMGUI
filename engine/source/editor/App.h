#pragma once
#include "Window.h"
#include "Timer.h"
#include "GameObject.h"
#include <memory>
class App
{
public:
    App();

    int Run();
private:
    void DoFrame();

    void UpdateGameObject(float deltaTime);

    void OnFrameUpdateBegin();
    void OnFrameUpdateEnd();

    // wnd pointer
    std::unique_ptr<Window> pWnd;
    Timer timer;

    std::vector<std::unique_ptr<GameObject>> pGameObjects;
};
