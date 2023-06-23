#include "App.h"
#include <iomanip>
#include "EngineException.h"
#ifdef _WIN32
#include "Win32Window.h"
#elif __linux__
#endif

#include "Material.h"
#include "Cube.h"
#include "Triangle.h"
#include "Camera.h"
App::App()
{

// if win32 , use win32 window
#ifdef _WIN32
    pWnd = std::make_unique<Win32Window>(1280, 720, L"TinyRenderer");
#elif __linux__
    pWnd = std::make_unique<LinuxWindow>(1280, 720, L"TinyRenderer");
#endif
    pGameObjects.emplace_back(std::make_unique<Cube>()); // here will use move constructor;
    auto curCam = std::make_unique<Camera>();
    curCam->SetAspect(1280.0f / 720.0f);
    curCam->transform.SetPosition({ 0.0f, 0.0f, -6.0f });
    curCam->transform.SetRotation({ 0.0f, 0.0f, 0.0f });
    FrameUniformBufferManager::BindCamera(curCam.get());
    pGameObjects.emplace_back(std::move(curCam));
    
}

int App::Run()
{
    for (auto& pGo : pGameObjects)
    {
        pGo->Init();
    }
    
    try
    {
        while (true)
        {
            if (const auto ecode = pWnd->ProcessMessages())
            {
                return *ecode;
            }
            DoFrame();
        }
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
    return -1;
}

void App::DoFrame()
{
    OnFrameUpdateBegin();
    
    // delta time
    auto deltaTime = timer.Mark();
    UpdateGameObject(deltaTime);

    OnFrameUpdateEnd();
}

void App::UpdateGameObject(float deltaTime)
{
    for (auto& pGo : pGameObjects)
    {
        pGo->OnUpdate(deltaTime);
    }
}

void App::OnFrameUpdateBegin()
{
    // set PerFrameUniformBuffer
    pWnd->Gfx()->OnFrameBegin();
}

void App::OnFrameUpdateEnd()
{
    pWnd->Gfx()->OnFrameEnd();
}