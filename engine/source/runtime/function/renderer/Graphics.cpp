#include "Graphics.h"
#include <iostream>
#include "managers/RenderQueueManager.h"
#include "Material.h"
#include "components/Camera.h"
#include "components/Light.h"
#define LOG(X) std::cout << X << std::endl;
std::queue<Material*> Graphics::m_WaitForLoadMaterials;

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
    }
    // update uniform buffer
    material.UpdateBuffer();
}

void Graphics::AddMaterial(Material* pMaterial)
{
    m_WaitForLoadMaterials.push(pMaterial);
}

void Graphics::OnFrameBegin()
{
    RenderQueueManager::Clear();
    ClearBuffer(0.0f, 0.0f, 0.0f);

    while(!m_WaitForLoadMaterials.empty())
    {
        auto pMat = m_WaitForLoadMaterials.front();
        m_WaitForLoadMaterials.pop();
        if(pMat == nullptr) continue;
        if(!pMat->HasLoadedShader())
        {
            pMat->LoadShader(*this);
        }
    }
}

void Graphics::OnFrameUpdate()
{
    if(m_pShaderHelper != nullptr)
    {
        Camera::UpdateCameraBuffer(*m_pShaderHelper);
        Light::UpdateLightBuffer(*m_pShaderHelper);
    }
    DrawAll();
}
void Graphics::OnFrameEnd()
{
    EndFrame();
}