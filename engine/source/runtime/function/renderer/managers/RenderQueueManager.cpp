#include "RenderQueueManager.h"

RenderQueueManager::RenderQueueManager()
{

}

RenderQueueManager::~RenderQueueManager()
{
    opaqueRenderQueue.clear();
    alphaTestRenderQueue.clear();
    transparentRenderQueue.clear();
}
std::unordered_map<Material *, std::vector<Renderer*>>& RenderQueueManager::GetOpaqueRenderQueue()
{
    return opaqueRenderQueue;
}

std::unordered_map<Material *, std::vector<Renderer*>>& RenderQueueManager::GetAlphaTestRenderQueue()
{
    return alphaTestRenderQueue;
}

std::unordered_map<Material *, std::vector<Renderer*>>& RenderQueueManager::GetTransparentRenderQueue()
{
    return transparentRenderQueue;
}

void RenderQueueManager::Add(Material* pMaterial, Renderer * pRenderer)
{
    if(pMaterial == nullptr) return;
    // switch (pMaterial->GetRenderingMode())
    // {
    // case ERenderingMode::Opaque:
    //     if(opaqueRenderQueue.find(pMaterial) == opaqueRenderQueue.end()) opaqueRenderQueue.emplace(pMaterial, std::vector<Renderer*>());
    //     opaqueRenderQueue[pMaterial].emplace_back(pRenderer);
    //     break;
    // case ERenderingMode::Cutout:
    //     if(alphaTestRenderQueue.find(pMaterial) == alphaTestRenderQueue.end()) alphaTestRenderQueue.emplace(pMaterial, std::vector<Renderer*>());
    //     alphaTestRenderQueue[pMaterial].emplace_back(pRenderer);
    //     break;
    // case ERenderingMode::Transparent:
    //     if(transparentRenderQueue.find(pMaterial) == transparentRenderQueue.end()) transparentRenderQueue.emplace(pMaterial, std::vector<Renderer*>());
    //     transparentRenderQueue[pMaterial].emplace_back(pRenderer);
    //     break;
    // default:
    //     if(opaqueRenderQueue.find(pMaterial) == opaqueRenderQueue.end()) opaqueRenderQueue.emplace(pMaterial, std::vector<Renderer*>());
    //         opaqueRenderQueue[pMaterial].emplace_back(pRenderer);
    // }
}
void RenderQueueManager::AddRenderer(Renderer * pRenderer)
{
    auto& manager = RenderQueueManager::Get();
    auto pMaterial = pRenderer->GetMaterialPtr();
    manager.Add(pMaterial, pRenderer);
}

void RenderQueueManager::Clear()
{
    auto& manager = RenderQueueManager::Get();
    manager.opaqueRenderQueue.clear();
    manager.alphaTestRenderQueue.clear();
    manager.transparentRenderQueue.clear();
}
