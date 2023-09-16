#include "engine.h"
#include "scene/SceneManager.h"
#include "world/WorldManager.h"
#include "surfaces/SurfaceUI.h"
#include "FrameBuffer.h"
#include "Graphics.h"
namespace TinyEngine
{
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
        
        // 3. initialize worldManager
        WorldManager::Get().Initialize();
        // 4. initialize sceneManager
        SceneManager::Get().Initialize();
        // 5. initialize inputSystem
        

        // initialize frame buffer
        m_framebuffer = new FrameBuffer();
    }

    void Engine::Run()
    {
        while (true)
        {
            float deltaTime = tick();
            logicalTick(deltaTime);
            if(!rendererTick()) return ;
        }
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
    }
    bool Engine::rendererTick()
    {
        // 1. render scene
        m_graphics->OnTick(m_framebuffer);
        // 2. render ui
        bool not_quit = m_surface_ui->OnTick(m_framebuffer->m_uistate.get());
        //release framebuffer
        m_graphics->EndFrame();
        return not_quit;
    }


    void Engine::ShutdownEngine()
    {
        //clear
        WorldManager::Get().Clear();

        SceneManager::Get().Clear();
    }
}

