#include "SceneManager.h"
#include "Scene.h"
#include "managers/RenderQueueManager.h"
#include "object/GameObject.h"
#include "components/Renderer.h"
#include "Material.h"
#include "FrameBuffer.h"
namespace TinyEngine
{
    SceneManager::SceneManager()
    {
    }

    SceneManager::~SceneManager()
    {
    }

    void SceneManager::Initialize()
    {
        if (m_scene && m_scene->m_loaded == false) m_scene->Load();
    }

    int SceneManager::Tick(FrameBuffer * pFrameBuffer)
    {
        // bind scene datas to framebuffer
        SyncSceneObjects();
        return 0;
    }

    int SceneManager::Clear()
    {
        if (m_scene) m_scene->Clear();
        return 0;
    }

    void SceneManager::SyncSceneObjects()
    {
        // every tick, we add all renderer to sceneManager
        
    }

    void SceneManager::AddRenderer(Renderer * pRenderer)
    {
        Material * pMaterial = pRenderer->GetMaterialPtr();
        if (pMaterial)
        {
            auto it = renderQueue.find(pMaterial);
            if (it == renderQueue.end())
            {
                renderQueue[pMaterial] = std::vector<Renderer*>();
            }
            renderQueue[pMaterial].push_back(pRenderer);
        }
    }
    void SceneManager::setSceneOnce()
    {

    }
} // namespace TinyEngine