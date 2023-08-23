#include "App.h"
#include <iomanip>
#include "core/EngineException.h"
#ifdef _WIN32
#include "Win32Window.h"
#elif __linux__
#endif

#include "Material.h"
#include "Texture.h"
#include "components/Camera.h"
#include "primitives/Primitive.h"
#include "behaviours/Rotater.h"
#include "behaviours/CamController.h"
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
    pGameObjects.clear();
}

void App::LoadGOs()
{
    pGameObjects.emplace_back(Primitive::CreateSkinedCube("cube1")); // here will use move constructor;
    pGameObjects.emplace_back(Primitive::CreateCube("cube2")); // here will use move constructor;
    
    auto camGO = std::make_unique<GameObject>("Cam");
    auto cam = camGO->AddComponent<Camera>();
    camGO->AddComponent<CamController>();
    cam->SetAspect(1280.0f / 720.0f);
    camGO->transform.SetPosition({ 0.0f, 0.0f, 6.0f });
    camGO->transform.SetEulerAngle({ 0.0f, 180.0f, 0.0f });
    pGameObjects.emplace_back(std::move(camGO));
    
    for (auto& pGo : pGameObjects)
    {
        pGo->Init();
    }
    pGameObjects[0]->transform.SetPosition({ 1.5f, 1.5f, 0.0f });
    pGameObjects[1]->transform.SetPosition({ -1.5f, -1.5f, 0.0f });
    pGameObjects[0]->AddComponent<Rotater>();
    pGameObjects[1]->AddComponent<Rotater>();
    auto pMat = Material::Load("shaders/DefaultVertexShader.hlsl", "shaders/TexPixelShader.hlsl");
    auto pRenderer1 = pGameObjects[0]->GetComponent<Renderer>();
    auto pRenderer2 = pGameObjects[1]->GetComponent<Renderer>();
    pRenderer1->SetSharedMaterial(pMat);
    pRenderer2->SetSharedMaterial(pMat);
    // auto pTexture = Texture::LoadFrom("res/images/cube.png");
    auto pCubTex = Texture::LoadFrom("res/images/cube.png");
    auto pBrickwallTex = Texture::LoadFrom("res/images/brickwall.jpg");
    pRenderer2->GetMaterial()->SetTexture("_MainTex", pBrickwallTex);
    pRenderer1->GetSharedMaterial()->SetTexture("_MainTex", pCubTex);
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

void App::UpdateGameObject(float deltaTime)
{
    for (auto& pGo : pGameObjects) 
        pGo->OnUpdate(deltaTime);
    for (auto& pGo : pGameObjects)
        pGo->OnLateUpdate(deltaTime);
}

void App::OnFrameUpdateBegin()
{
    // set PerFrameUniformBuffer
    pWnd->Gfx()->OnFrameBegin();
    for (auto& pGo : pGameObjects) 
        pGo->OnPreUpdate();
}

void App::OnFrameUpdateEnd()
{
    
    // imgui
    if( imgui.IsEnabled())
    {
        imgui.NewFrame();
        // static bool show_demo_window = true;
        // ImGui::ShowDemoWindow(&show_demo_window);

        for (auto& pGo : pGameObjects) pGo->OnGUI();

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