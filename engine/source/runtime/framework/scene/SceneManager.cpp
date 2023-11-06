#include "SceneManager.h"
#include "Scene.h"
#include "managers/RenderQueueManager.h"
#include "object/GameObject.h"
#include "components/Renderer.h"
#include "Material.h"
#include "FrameBuffer.h"
#include "effect/Pass.h"
#include "Texture.h"
#include "global/RuntimeGlobalContext.h"

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
        
        m_scene->m_directional_light.m_csm.m_split_num = g_runtime_global_context.m_shadow_config.m_directional_shadow_config.csm_split_num;
        
        m_map_resource = std::make_shared<SceneResource>();
        m_map_resource->m_skybox_cubemap = std::make_shared<CubeTexture>();
        for(int i = 0 ; i < 6; i++)
            m_map_resource->m_skybox_cubemap->textures[i] = Texture::LoadFrom(g_runtime_global_context.m_skybox_cubemap_path[i]);
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

        m_scene->p_map_resource = m_map_resource.get();
        if(m_selected_object != nullptr)
        {
            auto renderer = m_selected_object->GetComponent<Renderer>();
            if(renderer)
                m_scene->selectedRenderers.emplace_back(renderer);
            
            auto camera = m_selected_object->GetComponent<Camera>();
            if(camera)
                m_scene->selectedCamera = camera;
        }
        // update scene bounds
        //camera 
        m_scene->m_camera_list.cameras = Camera::cameraList;
        m_scene->m_main_camera = Camera::pActivedCamera;
        m_scene->m_renderers = std::move(m_renderers);
        //directional light
        m_scene->m_directional_light.exist = Light::GetDirectionalLight() != nullptr;
        if(m_scene->m_directional_light.exist)
        {
            m_scene->m_directional_light.use_csm = g_runtime_global_context.m_shadow_config.m_directional_shadow_config.use_csm;
            m_scene->m_directional_light.m_csm.m_split_lambda = g_runtime_global_context.m_shadow_config.m_directional_shadow_config.csm_split_lambda;
            m_scene->m_directional_light.m_buffer.m_direction = Light::GetDirectionalLight()->pTransform->forward();
            m_scene->m_directional_light.m_buffer.m_color = Light::GetDirectionalLight()->GetColor();
        }
        
        // point light
        auto pointLights = Light::GetPointLightList();
        m_scene->m_point_lights.resize(pointLights.size());
        for(size_t i = 0; i < m_scene->m_point_lights.size(); ++i)
        {
            PointLight pointLight;
            pointLight.m_buffer.pos = pointLights[i]->pTransform->GetPosition();
            pointLight.m_buffer.color = pointLights[i]->GetColor();
            pointLight.m_buffer.atten = 1.0f;
            pointLight.m_buffer.range = pointLights[i]->GetRange();
            
            pointLights[i]->GetLightView(pointLight.m_lightView);
            pointLights[i]->GetLightProj(&(pointLight.m_lightProj));
            for(int j = 0 ; j < 6; j++)
                pointLight.m_lightViewProj[j] = pointLight.m_lightProj * pointLight.m_lightView[j];
            m_scene->m_point_lights[i] = pointLight;
        }
        
        std::vector<EffectDesc>     effectDescs;
        std::vector<ShadowCastDesc> shadowCastDescs;
        // add all info(required for rendering) to scene
        for(auto & pair : effectQueueByPriority)
        {
            if(pair.second.size() == 0) continue;
            for(auto & effect : pair.second)
            {
                if(effect->CastShadow()) 
                {
                    shadowCastDescs.emplace_back(ShadowCastDesc{rendererQueue[effect]});
                    m_scene->shadowCastRenderers.insert(m_scene->shadowCastRenderers.end(), rendererQueue[effect].begin(), rendererQueue[effect].end());
                }
                
                effectDescs.emplace_back(EffectDesc{effect,rendererQueue[effect]});
            }
        }

        effectQueueByPriority.clear();
        rendererQueue.clear();
        m_renderers.clear();
        m_scene->updateVisibleObjects(effectDescs, shadowCastDescs);
        
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
            m_renderers.emplace_back(pRenderer);
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