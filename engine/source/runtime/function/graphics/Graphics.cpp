#include "Graphics.h"
#include <iostream>
#include "managers/RenderQueueManager.h"
#include "Material.h"
#include "components/Camera.h"
#include "components/Light.h"
#include "graph/RenderGraph.h"

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

