#include "Level.h"

#include "core/base/macro.h"
// #include "runtime/resource/asset_manager/asset_manager.h"
#include "res_type/common.h"

#include "object/GameObject.h"
#include "Material.h"
#include "Texture.h"
#include "geometry/Primitive.h"
#include "behaviours/behaviours.h"
#include "components/components.h"

#include <limits>
#include <assert.h>

namespace TinyEngine
{
    Level::~Level() { 
        Clear(); 
    }

    
/************************************************************************************************/
/*                                         Private Methods                                      */
/************************************************************************************************/
GameObject* Level::addGameObject(GameObject* pGo)
    {
        if (pGo == nullptr) return nullptr;
        pGameObjectsToCreate.emplace_back(pGo);
        return pGo;
    }
    void Level::processPendingCreateGameObjects()
    {
        for (auto& pGo : pGameObjectsToCreate)
        {
            assert(pGo);
            if (!pGo) continue;
            pGameObjects.emplace(pGo->GetInstanceID(), pGo);
            pGosByName[pGo->GetName()].emplace_back(pGo);
        }
        pGameObjectsToCreate.clear();
    }

    void Level::processPendingDestroyGameObjects()
    {
        for (auto& pGo : pGameObjectsToDestroy)
        {
            assert(pGo);
            if (!pGo) continue;
            auto iter = pGameObjects.find(pGo->GetInstanceID());
            if (iter != pGameObjects.end()) pGameObjects.erase(iter);

            auto& gos = pGosByName[pGo->GetName()];
            auto iter2 = std::find(gos.begin(), gos.end(), pGo);
            if (iter2 != gos.end()) gos.erase(iter2);
            delete pGo;
        }
        pGameObjectsToDestroy.clear();
    }
    
/************************************************************************************************/
/*                                         Public Methods                                      */
/************************************************************************************************/
    

    void Level::Tick(const float delta_time)
    {
        // deal gameobjects: create and destroy
        processPendingCreateGameObjects();
        processPendingDestroyGameObjects();

        for (const auto& id_object_pair : pGameObjects)
        {
            auto & pGo = id_object_pair.second;
            assert(pGo);
            if (!pGo) continue;
            if(!pGo->IsActived() || pGo->IsInitialized()) continue;
            pGo->Init();
        }

        for (const auto& id_object_pair : pGameObjects)
        {
            assert(id_object_pair.second);
            if (id_object_pair.second)
            {
                id_object_pair.second->OnPreUpdate();
            }
        }

        for (const auto& id_object_pair : pGameObjects)
        {
            assert(id_object_pair.second);
            if (id_object_pair.second)
            {
                id_object_pair.second->OnUpdate(delta_time);
            }
        }

        for (const auto& id_object_pair : pGameObjects)
        {
            assert(id_object_pair.second);
            if (id_object_pair.second)
            {
                id_object_pair.second->OnLateUpdate(delta_time);
            }
        }
    }
    GameObject* Level::Find(const std::string& name) const
    {
        auto iter = pGosByName.find(name);
        if (iter != pGosByName.end())
        {
            if(iter->second.size() > 0)
                return iter->second[0];
        }
        return nullptr;
    }
    GameObject* Level::CreateGameObject(std::string name)
    {
        GameObjectRes go_res;
        go_res.m_name = name;
        return CreateGameObject(go_res);
    }
    GameObject* Level::CreateGameObject(const GameObjectRes& go_instance_res)
    {
        size_t   gobject_id = INVALID_GO_ID;
        GameObject* pGo    = new GameObject(m_next_gobject_id);
        assert(pGo);
        if (pGo == nullptr)
        {
            LOG_ERROR("cannot allocate memory for new gobject");
        }
        else
        {
            bool is_loaded = pGo->loadres(go_instance_res);
            assert(is_loaded);
            if (is_loaded)
            {
                gobject_id = m_next_gobject_id;
                ++m_next_gobject_id;
            }
            else
            {
                LOG_ERROR("loading object " + go_instance_res.m_name + " failed");
                delete pGo;
                return nullptr;
            }
        }
        pGameObjectsToCreate.emplace_back(pGo);
        return pGo;
    }

    void Level::DestroyGameObject(GameObject* pGo)
    {
        assert(pGo);
        if (!pGo) return;
        // destroy all children
        if(pGo->transform().GetChildCount() > 0)
        {
            for(int i = 0; i < pGo->transform().GetChildCount(); i++) 
                DestroyGameObject(pGo->transform().GetChild(i)->GetGameObject());
        }
        // add to destroy list
        pGameObjectsToDestroy.emplace_back(pGo);
    }
    void Level::DestroyGameObject(const size_t go_id)
    {
        auto iter = pGameObjects.find(go_id);
        if (iter != pGameObjects.end()) DestroyGameObject(iter->second);
    }

    GameObject* Level::GetGameObjectByID(const size_t go_id) const
    {
        auto iter = pGameObjects.find(go_id);
        if (iter != pGameObjects.end()) return iter->second;
        return nullptr;
    }

    void Level::Clear()
    {
        for (auto& id_gobject_pair : pGameObjects)
        {
            GameObject* go = id_gobject_pair.second;
            assert(go);
            if (go) delete go;
        }
        pGameObjects.clear();
        pGosByName.clear();
        pGameObjectsToDestroy.clear();
        
        for (auto& pGo : pGameObjectsToCreate)
        {
            assert(pGo);
            if (pGo) delete pGo;
        }

        pGameObjectsToCreate.clear();

        // for(auto& go : pGameObjects)
        // {
        //     go.release();
        // }
        // pRootGameObjects.clear();
        // pGameObjects.clear();
        // goIdsByName.clear();
    }

    void Level::Load(const std::string& level_res_url)
    {
        m_level_res_url = level_res_url;
        
        throw std::logic_error("The method or operation is not implemented.");
        // AssetManager& asset_manager = AssetManager::Get();

        // LevelRes level_res;
        // asset_manager.loadAsset(asset_manager.getFullPath(level_res_url), level_res);

        // for (const ObjectInstanceRes& object_instance_res : level_res.m_objects)
        // {
        //     createObject(object_instance_res);
        // }
    }

    void Level::Save()
    {
        throw std::logic_error("The method or operation is not implemented.");
        // LevelRes output_level_res;

        // const size_t                    object_cout    = m_gobjects.size();
        // std::vector<ObjectInstanceRes>& output_objects = output_level_res.m_objects;
        // output_objects.resize(object_cout);

        // size_t object_index = 0;
        // for (const auto& id_object_pair : m_gobjects)
        // {
        //     assert(id_object_pair.second);
        //     if (id_object_pair.second)
        //     {
        //         id_object_pair.second->save(output_objects[object_index]);
        //         ++object_index;
        //     }
        // }

        // AssetManager& asset_manager = AssetManager::getInstance();
        // asset_manager.saveAsset(output_level_res, asset_manager.getFullPath(m_level_res_url));
    }

    void Level::LoadDefault()
    {
        auto pBrickwallTex = Texture::LoadFrom("res/images/brickwall.jpg");
        auto pCubTex = Texture::LoadFrom("res/images/cube.png");
        auto pMat = Material::GetDefaultMaterialPtr();

        // set camera
        auto camGO = CreateGameObject("Cam");
        auto m_main_camera = camGO->AddComponent<Camera>();
        camGO->AddComponent<CamController>();
        

        auto plane = addGameObject(Primitive::CreatePlane("plane"));
        // set plane
        plane->transform().SetPosition({ 0.0f, -1.0f, 0.0f });
        plane->transform().SetScale({ 2.0f, 1.0f, 2.0f });
        plane->GetComponent<Renderer>()->SetMaterial(pMat);


        // auto gameController = CreateGameObject("GameController");
        // gameController->AddComponent<GameController>();

        auto lightController = CreateGameObject("LightController");
        lightController->AddComponent<LightController>();
        
        
        auto cube2 = addGameObject(Primitive::CreateCube("cube2"));
        cube2->transform().SetPosition({ -1.5f, 0.0f, 0.0f });
        cube2->AddComponent<Rotater>();
        auto pRenderer2 = cube2->GetComponent<Renderer>();
        pRenderer2->SetSharedMaterial(pMat);
        pRenderer2->GetMaterial()->SetTexture("_MainTex", pBrickwallTex);    
        return ;
    }

    void Level::LoadSponzaScene()
    {
        auto camGO = CreateGameObject("Cam");
        auto m_main_camera = camGO->AddComponent<Camera>();
        camGO->AddComponent<CamController>();
        
        camGO->pTransform->SetPosition({ 0.0f, 100.0f, -6.0f });
        camGO->pTransform->SetEulerAngle({ 30.0f, 0.0f, 0.0f });

        auto lightController = CreateGameObject("LightController");
        lightController->AddComponent<LightController>();

        auto sponza = addGameObject(GameObject::CreateFromFile("res/models/sponza/sponza.obj"));

        auto plane = addGameObject(Primitive::CreatePlane("plane"));
        plane->transform().SetPosition({ 100.0f, 100.0f, 0.0f });
        plane->transform().SetScale({ 4.0f, 1.0f, 4.0f });
        plane->transform().SetEulerAngle({ 90.0f, 0.0f, 0.0f });
        auto pTransparentMat = Material::CreateDefault("Plant Material", ERenderingMode::Transparent);
        plane->GetComponent<Renderer>()->SetMaterial(pTransparentMat);

        auto cube2 = addGameObject(Primitive::CreateCube("cube"));
        cube2->transform().SetPosition({ 0.0f, 100.0f, 0.0f });
        cube2->transform().SetScale({ 3.0f, 3.0f, 3.0f });
        auto pRenderer2 = cube2->GetComponent<Renderer>();
        auto pMat = Material::GetDefaultMaterialPtr();
        pRenderer2->SetSharedMaterial(pMat);
        auto pBrickwallTex = Texture::LoadFrom("res/images/brickwall.jpg");
        pRenderer2->GetMaterial()->SetTexture("_MainTex", pBrickwallTex);   
    }
/************************************************************************************************/
/*                                         Static Methods                                      */
/************************************************************************************************/
    

} // namespace TinyEngine
