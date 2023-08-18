#include "Graphics.h"
#include <iostream>
#include "MaterialManager.h"
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
    for(auto& pair : MaterialManager::GetMatRendererPair())
    {
        auto& mat = pair.first;
        auto& renderers = pair.second;
        mat->ClearUniformBuffer();

        for (auto& renderer : renderers)
            renderer.get().ClearObjBuffer();
    }
    ClearBuffer(0.0f, 0.0f, 0.0f);
}

void Graphics::OnFrameEnd()
{
    FrameUniformBufferManager::UpdateFrameBuffer();
    for(auto & pair : MaterialManager::GetMatRendererPair())
    {
        auto & mat = pair.first;
        auto & renderers = pair.second;
        mat->UpdateUniformBuffer();

        for (auto& renderer : renderers)
            renderer.get().UpdateObjBuffer();
    }

    DrawAll();
    // DrawTestTriangle();
    EndFrame();
}