#include "SceneManager.h"
#include "Scene.h"
#include "managers/RenderQueueManager.h"
#include "object/GameObject.h"
#include "components/Renderer.h"
#include "Material.h"
#include "FrameBuffer.h"
#include "effect/Pass.h"
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

        pFrameBuffer->m_scene = m_scene;
        pFrameBuffer->m_selectedObject = m_selected_object;
        // pFrameBuffer->m_uistate->
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
        m_scene->Lock();
        m_scene->Clear();

        if(m_selected_object != nullptr)
        {
            auto renderer = m_selected_object->GetComponent<Renderer>();
            if(renderer)
                m_scene->selectedRenderers.emplace_back(renderer);
        }
        //camera 
        m_scene->m_main_camera = Camera::pActivedCamera;
        
        //directional light
        m_scene->m_directional_light.m_buffer.m_direction = Light::GetDirectionalLight()->pTransform->forward();
        m_scene->m_directional_light.m_buffer.m_color = Light::GetDirectionalLight()->GetColor();
        m_scene->m_directional_light.m_lightVP = Light::GetDirectionalLight()->GetLightVP();
        
        // point light
        auto pointLights = Light::GetPointLightList();
        m_scene->m_point_lights.resize(pointLights.size());
        for(size_t i = 0; i < m_scene->m_point_lights.size(); ++i)
        {
            PointLight pointLight;
            pointLight.pos = pointLights[i]->pTransform->GetPosition();
            pointLight.color = pointLights[i]->GetColor();
            pointLight.atten = 1.0f;
            pointLight.range = pointLights[i]->GetRange();
            m_scene->m_point_lights[i] = pointLight;
        }

        // add all info(required for rendering) to scene
        for(auto & pair : effectQueueByPriority)
        {
            if(pair.second.size() == 0) continue;
            for(auto & effect : pair.second)
            {
                if(effect->CastShadow()) 
                    m_scene->ShadowCastDescs.emplace_back(
                        ShadowCastDesc{rendererQueue[effect]
                    });
                
                m_scene->effectDescs.emplace_back(
                    EffectDesc{
                        effect,
                        rendererQueue[effect]
                });
            }
        }

        effectQueueByPriority.clear();
        rendererQueue.clear();
        m_scene->Unlock();
    }

    void SceneManager::AddRenderer(Renderer * pRenderer)
    {
        Material * pMaterial = pRenderer->GetMaterialPtr();
        if (pMaterial)
        {
            auto pEffect = pMaterial->GetEffect();
            effectQueueByPriority[pEffect->queuePriority].emplace(pEffect.get());
            rendererQueue[pEffect.get()].push_back(pRenderer);
        }
    }
    void SceneManager::SetSelectedGameObject(GameObject * pGameObject) {
        if(m_selected_object == pGameObject) return ;
        m_selected_object = pGameObject;
        if(m_selected_object == nullptr) return ;
    }
    void SceneManager::setSceneOnce()
    {

    }
} // namespace TinyEngine