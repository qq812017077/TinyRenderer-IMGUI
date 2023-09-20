#pragma once

#include <string>
#include <unordered_map>

class GameObject;
namespace TinyEngine
{
    struct GameObjectRes;
    class Level
    {
    protected:
        size_t                               m_next_gobject_id {0};
        std::string                          m_level_res_url;
        std::unordered_map<size_t, GameObject*> pGameObjects;
        std::unordered_map<std::string, std::vector<GameObject*>> pGosByName;
        std::vector<GameObject*> pGameObjectsToDestroy;
        std::vector<GameObject*> pGameObjectsToCreate;

        GameObject* addGameObject(GameObject* pGo);
        void processPendingDestroyGameObjects();
        void processPendingCreateGameObjects();

    public:
        ~Level();
        void Tick(const float delta_time);

        GameObject*     Find(const std::string& name) const;
        GameObject*     CreateGameObject(std::string name="GameObject");
        GameObject*     CreateGameObject(const GameObjectRes& object_instance_res);
        void            DestroyGameObject(GameObject* pGo);
        void            DestroyGameObject(const size_t go_id);

        const std::string& GetLevelResUrl() const { return m_level_res_url; }
        const std::unordered_map<size_t, GameObject*>& GetAllGameObjects() const { return pGameObjects; }

        GameObject*                                    GetGameObjectByID(const size_t go_id) const;

        void Clear();
        void Load(const std::string& level_res_url);
        void Save();

        void LoadDefault();
        void LoadSponzaScene();
    };
} // namespace Pilot
