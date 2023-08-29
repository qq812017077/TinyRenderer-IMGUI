#include "RenderQueueManager.h"

RenderQueueManager::RenderQueueManager()
{

}

RenderQueueManager::~RenderQueueManager()
{
    renderQueue.clear();
}
std::map<Material *, std::vector<Renderer*>> RenderQueueManager::GetRenderQueue()
{
    auto& manager = RenderQueueManager::Get();
    return manager.renderQueue;
}
void RenderQueueManager::AddMaterial(Material* pMaterial)
{
    auto& manager = RenderQueueManager::Get();
    if(manager.renderQueue.find(pMaterial) != manager.renderQueue.end()) return;
    manager.renderQueue.emplace(pMaterial, std::vector<Renderer*>());
    for(auto& pair : pMaterial->rendererRefCountMap)
        manager.renderQueue[pMaterial].emplace_back(pair.first);
}

void RenderQueueManager::Clear()
{
    auto& manager = RenderQueueManager::Get();
    manager.renderQueue.clear();
}

void RenderQueueManager::Sort()
{
    auto& manager = RenderQueueManager::Get();

}