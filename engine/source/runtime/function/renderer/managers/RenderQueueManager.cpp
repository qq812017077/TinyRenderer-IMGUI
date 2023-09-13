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
    {
        auto & pRenderer = pair.first;
        if(!pRenderer->IsVisible()) continue;
        manager.renderQueue[pMaterial].emplace_back(pRenderer);
    }
}
void RenderQueueManager::AddRenderer(Renderer * pRenderer)
{
    auto& manager = RenderQueueManager::Get();
    auto pMaterial = pRenderer->GetMaterialPtr();
    if(manager.renderQueue.find(pMaterial) == manager.renderQueue.end())
    {
        manager.renderQueue.emplace(pMaterial, std::vector<Renderer*>());
    }
    manager.renderQueue[pMaterial].emplace_back(pRenderer);
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