#pragma once
#include "core/Singleton.h"
#include <unordered_map>
#include <vector>
#include <map>
#include <set>
#include "SceneObject.h"
#include "core/math/Vector.h"
#include "RenderContext.h"

class Material;
class Renderer;
class GameObject;
class Camera;
namespace TinyEngine
{
    class Scene;
    class FrameBuffer;
    class Effect;

    class SceneManager final : public Singleton<SceneManager>
    {
    public:
        SceneManager();
        ~SceneManager();

        void Initialize();
        int Tick(FrameBuffer * pFrameBuffer);
        int  Clear();

        std::shared_ptr<Scene> getCurrentScene() const { return m_scene; }
        void SyncSceneObjects();

        void AddRenderer(Renderer * pRenderer);

        void SetSelectedGameObject(GameObject * pGameObject);
    protected:
        std::shared_ptr<Scene>             m_scene = std::make_shared<Scene>();
        std::unordered_map<size_t, size_t> m_mesh_id_gobejct_id_map;

        GameObject * m_selected_object = nullptr;

        SceneResourceDesc                   m_resource_desc;
        std::shared_ptr<SceneResource>      m_map_resource;
    private:
        std::vector<GameObjectDesc>         m_go_descs;
        
        Camera* m_actived_camera{nullptr};
        std::map<int, std::set<Effect *>> effectQueueByPriority;
        std::map<Effect*, std::vector<Renderer*>> rendererQueue;
        std::vector<Renderer *> m_renderers;
        void setSceneOnce();

    };
} // namespace TinyEngine