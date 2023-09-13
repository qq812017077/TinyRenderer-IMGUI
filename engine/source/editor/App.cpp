#include "App.h"
#include <iomanip>
#include "core/EngineException.h"
#ifdef _WIN32
#include "Win32Window.h"
#elif __linux__
#endif
#include "Material.h"
#include "Texture.h"
#include "geometry/Primitive.h"
#include "behaviours/Rotater.h"
#include "behaviours/CamController.h"
#include "behaviours/LightController.h"
#include "behaviours/GameController.h"
#include "components/Light.h"
App::App()
{

// if win32 , use win32 window
#ifdef _WIN32
    pWnd = std::make_unique<Win32Window>(1280, 720, L"TinyRenderer");
    imgui.Init(dynamic_cast<Win32Window *>(pWnd.get()));
#elif __linux__
    pWnd = std::make_unique<LinuxWindow>(1280, 720, L"TinyRenderer");
#endif

}

App::~App()
{
    //clear all the game objects
}

void App::LoadGOs()
{
    // load resource
    auto pMat = Material::Load("shaders/DefaultVertexShader.hlsl", "shaders/PhongPS.hlsl");
    auto pCubTex = Texture::LoadFrom("res/images/cube.png");
    auto pBrickwallTex = Texture::LoadFrom("res/images/brickwall.jpg");

    auto gameController = GameObject::CreateGameObject("GameController");
    gameController->AddComponent<GameController>();

    auto lightController = GameObject::CreateGameObject("LightController");
    lightController->AddComponent<LightController>();

    auto plane = Primitive::CreatePlane("plane");
    auto pointLight = Light::CreatePointLight("pointLight");
    
    
    // set camera
    auto camGO = GameObject::CreateGameObject("Cam");
    auto cam = camGO->AddComponent<Camera>();
    camGO->AddComponent<CamController>();

    // set plane
    plane->transform.SetPosition({ 0.0f, -1.0f, 0.0f });
    plane->transform.SetScale({ 2.0f, 1.0f, 2.0f });
    plane->GetComponent<Renderer>()->SetMaterial(pMat);

    
    auto cube2 = Primitive::CreateCube("cube2");// here will use move constructor;
    cube2->transform.SetPosition({ -1.5f, 0.0f, 0.0f });
    cube2->AddComponent<Rotater>();
    auto pRenderer2 = cube2->GetComponent<Renderer>();
    pRenderer2->SetSharedMaterial(pMat);
    pRenderer2->GetMaterial()->SetTexture("_MainTex", pBrickwallTex);
}

int App::Run()
{
    try
    {
        // return 0;
        LoadGOs();
        while (true)
        {
            pWnd->Update();
            if (const auto ecode = pWnd->ProcessMessages())
            {
                // if the window is closed, the return value is 0
                if (*ecode == 0)
                {
                    // clear all the game objects
                    GameObject::RemoveAllGameObjects();
                    return 0;
                }
                return *ecode;
            }
            DoFrame();
        }
    }
    catch(const EngineException& e)
    {
        pWnd->ShowMessageBox(e.what(), e.GetType());
        // MessageBox(pWnd->GetWindowHandle(), e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
        // focus on the message box
        // SetForegroundWindow(pWnd->GetWindowHandle());
    }
    catch(const std::exception& e)
    {
        pWnd->ShowMessageBox(e.what(), "Standard Exception");
        // MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch(...)
    {
        pWnd->ShowMessageBox("No details available", "Unknown Exception");
        // MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    return -1;
}

void App::DoFrame()
{
    auto deltaTime = timer.Mark();
    OnFrameUpdateBegin();
    UpdateGameObject(deltaTime);
    pWnd->Gfx()->OnFrameUpdate();
    OnFrameUpdateEnd();
}


void App::OnFrameUpdateBegin()
{
    // set PerFrameUniformBuffer
    GameObject::RefreshQueue();
    for (auto& pGo : GameObject::GetRootGameObjects())
    {
        if(!pGo->IsActived()) continue;
        if(pGo->IsInitialized()) continue;
        pGo->Init();
    }
    
    pWnd->Gfx()->OnFrameBegin();
    for (auto& pGo : GameObject::GetRootGameObjects())
    {
        if(!pGo->IsActived()) continue;
        pGo->OnPreUpdate();
    }
}

void App::UpdateGameObject(float deltaTime)
{
    for (auto& pGo : GameObject::GetRootGameObjects())
    {
        if(!pGo->IsActived()) continue;
        pGo->OnUpdate(deltaTime);
    }
    for (auto& pGo : GameObject::GetRootGameObjects())
    {
        if(!pGo->IsActived()) continue;
        pGo->OnLateUpdate(deltaTime);
    }
}

void App::OnFrameUpdateEnd()
{
    // imgui
    if( imgui.IsEnabled())
    {
        imgui.NewFrame();
        // static bool show_demo_window = true;
        // ImGui::ShowDemoWindow(&show_demo_window);

        for (auto& pGo : GameObject::GetRootGameObjects()) pGo->OnGUI();

        if(ImGui::Begin("FPS"))
        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
        ImGui::End();
        imgui.Render();
    }
    pWnd->Gfx()->OnFrameEnd();
}