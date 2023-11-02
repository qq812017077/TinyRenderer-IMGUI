#include "Engine.h"
#include "scene/SceneManager.h"
#include "effect/EffectManager.h"
#include "world/WorldManager.h"
#include "input/Input.h"
#include "FrameBuffer.h"
#include "Graphics.h"
#include "global/RuntimeGlobalContext.h"
namespace TinyEngine
{
    bool g_is_editor_mode {true};
    Engine::Engine()
    {
    }

    Engine::~Engine()
    {
    }

    /// @brief initialize environments
    void Engine::Initialize()
    {
        // 1. initialize config

        // 2. initialize assetManager
        
        // 3. Initialize EffectManager
        EffectManager::Get().Initialize();
        
        // 4. initialize worldManager
        WorldManager::Get().Initialize();
        // 5. initialize sceneManager
        SceneManager::Get().Initialize();
        // 6. initialize inputSystem
        Input::InputSystem::Get().Initialize();

        // initialize frame buffer
        m_framebuffer = new FrameBuffer();
    }

    
    bool Engine::tickOneFrame(float deltaTime)
    {
        logicalTick(deltaTime);
        if(!rendererTick()) return false;
        return true;
    }


    float Engine::tick()
    {
        return timer.Mark();
    }
    void Engine::logicalTick(float deltaTime)
    {
        // world manager
        WorldManager::Get().Tick(deltaTime);
        // scene manager
        SceneManager::Get().Tick(m_framebuffer);
        // input system
        Input::InputSystem::Get().Tick();
    }
    bool Engine::rendererTick()
    {
        // 1. render scene
        m_graphics->OnTick(m_framebuffer);
        // 2. render ui
        bool not_quit = m_surface_ui->OnTick(m_framebuffer->m_uistate.get());
        m_graphics->EndFrame();
        //release framebuffer
        return not_quit;
    }


    void Engine::ShutdownEngine()
    {
        //clear
        WorldManager::Get().Clear();

        SceneManager::Get().Clear();
    }
}

