#include "Graphics.h"
#include <iostream>
#include "managers/RenderQueueManager.h"
#include "Material.h"
#define LOG(X) std::cout << X << std::endl;
Graphics::Graphics()
{
    LOG("base Graphics  constructor begin")
    // CreateDevice();
    // CreateRenderTarget();
    LOG("base Graphics  constructor end")
}

Graphics::~Graphics()
{
    // ClearupDevice();
}

void Graphics::LoadMaterial(Material& material)
{
    // load shader if not loaded
    if(!material.HasLoadedShader())
    {
        material.LoadShader(*this);
        // update uniform buffer
        material.UpdateUniformBuffer();
    }
}

void Graphics::OnFrameBegin()
{
    FrameUniformBufferManager::ClearFrameBuffer();
    RenderQueueManager::Clear();
    ClearBuffer(0.0f, 0.0f, 0.0f);
}

void Graphics::OnFrameUpdate()
{
    FrameUniformBufferManager::UpdateFrameBuffer();
    for(auto & pair : RenderQueueManager::GetRenderQueue())
    {
        auto & mat = pair.first;
        mat->UpdateUniformBuffer();
        for(auto & renderer : pair.second)
        {
            renderer->UpdateObjBuffer();
        }
    }
    DrawAll();
}
void Graphics::OnFrameEnd()
{
    EndFrame();
}