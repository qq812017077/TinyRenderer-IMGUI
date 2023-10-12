#pragma once
#include "core/Singleton.h"
#include "components/Renderer.h"
#include "Material.h"
#include <vector>
#include <unordered_map>

class RenderQueueManager: public Singleton<RenderQueueManager>
{
public:
    RenderQueueManager();
    ~RenderQueueManager();
    RenderQueueManager(RenderQueueManager const&) = delete;
    RenderQueueManager& operator=(RenderQueueManager const&) = delete;

    std::unordered_map<Material *, std::vector<Renderer*>>& GetOpaqueRenderQueue();
    std::unordered_map<Material *, std::vector<Renderer*>>& GetAlphaTestRenderQueue();
    std::unordered_map<Material *, std::vector<Renderer*>>& GetTransparentRenderQueue();
    void Add(Material* pMaterial, Renderer * pRenderer);
    static void AddRenderer(Renderer * pRenderer);
    static void Clear();
    static void Sort();

private:
    std::unordered_map<Material *, std::vector<Renderer*>> opaqueRenderQueue;
    std::unordered_map<Material *, std::vector<Renderer*>> alphaTestRenderQueue;
    std::unordered_map<Material *, std::vector<Renderer*>> transparentRenderQueue;
};