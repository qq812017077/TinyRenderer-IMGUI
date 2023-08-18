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
#include "components/Rotater.h"
#include "primitives/Primitive.h"
App::App()
{

// if win32 , use win32 window
#ifdef _WIN32
    pWnd = std::make_unique<Win32Window>(1280, 720, L"TinyRenderer");
#elif __linux__
    pWnd = std::make_unique<LinuxWindow>(1280, 720, L"TinyRenderer");
#endif
}

void App::LoadGOs()
{
    // pGameObjects.emplace_back(Primitive::CreateSimpleCube("cube1")); // here will use move constructor;
    pGameObjects.emplace_back(Primitive::CreateSkinedCube("cube1")); // here will use move constructor;
    pGameObjects.emplace_back(Primitive::CreateCube("cube2")); // here will use move constructor;
    
    auto curCam = std::make_unique<Camera>();
    curCam->SetAspect(1280.0f / 720.0f);
    curCam->transform.SetPosition({ 0.0f, 0.0f, 6.0f });
    curCam->transform.SetRotation({ 0.0f, 180.0f, 0.0f });
    pGameObjects.emplace_back(std::move(curCam));
    
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
    pRenderer2->GetSharedMaterial()->SetTexture("_MainTex", pBrickwallTex);
    pRenderer1->GetSharedMaterial()->SetTexture("_MainTex", pCubTex);
}

int App::Run()
{
    try
    {
        LoadGOs();
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