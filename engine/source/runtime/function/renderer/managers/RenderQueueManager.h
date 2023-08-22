#pragma once
#include "core/Singleton.h"
#include "components/Renderer.h"
#include "Material.h"
#include <vector>

class RenderQueueManager: public Singleton<RenderQueueManager>
{
public:
    RenderQueueManager();
    ~RenderQueueManager();
    RenderQueueManager(RenderQueueManager const&) = delete;
    RenderQueueManager& operator=(RenderQueueManager const&) = delete;

    static std::map<Material *, std::vector<Renderer*>> GetRenderQueue();
    static void AddMaterial(Material * pMaterial);
    static void Clear();
    static void Sort();

private:
    std::map<Material *, std::vector<Renderer*>> renderQueue;
};