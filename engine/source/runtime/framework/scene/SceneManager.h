#pragma once
#include "core/Singleton.h"
#include <unordered_map>
#include <vector>
#include "SceneObject.h"

class Material;
class Renderer;
namespace TinyEngine
{
    class Scene;
    class FrameBuffer;
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
    protected:
        std::shared_ptr<Scene>             m_scene = std::make_shared<Scene>();
        std::unordered_map<size_t, size_t> m_mesh_id_gobejct_id_map;
    
    private:
        std::vector<GameObjectDesc> m_go_descs;
        std::unordered_map<Material *, std::vector<Renderer*>> renderQueue;
        void setSceneOnce();
    };
} // namespace TinyEngine