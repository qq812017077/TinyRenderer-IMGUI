#pragma once
#include "Window.h"
#include "core/Timer.h"
#include "core/GameObject.h"
#include <memory>
#include "ImguiManager.h"
class App
{
public:
    App();
    ~App();
    int Run();
private:
    void LoadGOs();
    void DoFrame();

    void UpdateGameObject(float deltaTime);

    void OnFrameUpdateBegin();
    void OnFrameUpdateEnd();

    // wnd pointer
    std::unique_ptr<Window> pWnd;
    Timer timer;
    ImguiManager imgui;
    std::vector<std::unique_ptr<GameObject>> pGameObjects;
};
