#include "Graphics.h"
#include <iostream>
#include "managers/RenderQueueManager.h"
#include "Material.h"
#include "components/Camera.h"
#include "components/Light.h"
#include "graph/RenderGraph.h"
#include "scene/Scene.h"

#define LOG(X) std::cout << X << std::endl;
// std::queue<Material*> Graphics::m_WaitForLoadMaterials;

Graphics::Graphics()
{
    LOG("base Graphics  constructor begin")
    LOG("base Graphics  constructor end")
}

Graphics::~Graphics()
{
}




void Graphics::OnTick(TinyEngine::FrameBuffer * pFrameBuffer)
{
    // process swap data between logic and render contexts
    TinyEngine::EffectManager::Get().LoadShaders(this);

    // prepare render command context

    // update per-frame buffer

    // update per-frame visible objects


    // prepare pipeline's render passes data ( by render graph )
    
    UpdateSceneResource(pFrameBuffer->m_scene->p_map_resource);
    // render one frame ( by render graph )
    if(m_pRenderGraph == nullptr)
    {
    }else
    {
        m_pRenderGraph->BindScene(pFrameBuffer->m_scene);

        m_pRenderGraph->RefreshResource(this);
        m_pRenderGraph->Execute(this);
    }
    
    
}


void Graphics::UpdateSceneResource(TinyEngine::SceneResource * pSceneResource)
{
    if(pSceneResource == nullptr) return;
    if(pSceneResource->m_irradiance_cubemap == nullptr)
        pSceneResource->m_irradiance_cubemap = RenderIrradianceMap(pSceneResource->m_skybox_cubemap);
    
    if(pSceneResource->m_prefilter_cubemap == nullptr)
        pSceneResource->m_prefilter_cubemap = GeneratePrefilterMap(pSceneResource->m_skybox_cubemap);
    
    if(pSceneResource->m_brdf_lut_map == nullptr)
        pSceneResource->m_brdf_lut_map = GenerateBRDFLUT();
}
