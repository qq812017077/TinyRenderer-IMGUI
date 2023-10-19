#include "DirectXRenderGraph.h"
#include "DirectXGraphics.h"
#include "graph/RenderPass.h"
#include "graph/Attachment.h"
#include "graph/RenderResource.h"
#include "passlib.h"

namespace TinyEngine::Graph
{
    DXDefaultRenderGraph::DXDefaultRenderGraph(DirectXGraphics * pGfx)
    : RenderGraph(pGfx)
    {
        // AddGlobalSource(std::make_shared<RenderResource<DirectXRenderTarget>>("colorbuffer", ResourceDesc::RenderTarget()));
        mainRTHandle = CreateResource<DirectXRenderTarget>("mainRT", ResourceDesc::RenderTarget());
        effectRTHandle = CreateResource<DirectXRenderTarget>("postRT", ResourceDesc::RenderTarget());
        backBufferHandle = CreateResource<DirectXRenderTarget>("backbuffer", ResourceDesc::SwapChain());
        depthStencilHandle = CreateResource<DirectXDepthStencil>("depthstencil", ResourceDesc::DepthStencil());
        shadowMapHandle = CreateResource<DirectXDepthStencil>("shadowmap", ResourceDesc::ShadowMap());
        AddGlobalSource(backBufferHandle);
        AddGlobalSource(mainRTHandle);
        AddGlobalSource(effectRTHandle);
        AddGlobalSource(depthStencilHandle);
        AddGlobalSource(shadowMapHandle);
        AddGlobalSink(backBufferHandle);
        // AddGlobalSource(CreateResource<DirectXRenderTarget>("colorbuffer", ResourceDesc::RenderTarget()));
        // AddGlobalSource(std::make_shared<RenderResource<DirectXDepthStencil>>("depthstencil", ResourceDesc::DepthStencil()));
        // AddGlobalSink(std::make_shared<RenderResource<DirectXRenderTarget>>("backbuffer", ResourceDesc::SwapChain()));
        
        auto cleanRTPass = std::make_unique<BufferPass>("cleanRT");
        auto cleanDSPass = std::make_unique<BufferPass>("cleanDS");
        auto cleanSMPass = std::make_unique<BufferPass>("cleanSM");
        auto shadowPass = std::make_unique<ShadowPass>("shadow-cast");
        auto lightingPass = std::make_unique<LambertPass>("lighting");
        auto skyboxPass = std::make_unique<SkyBoxPass>("skybox");
        auto postPass = std::make_unique<PostProcessPass>("post-process");
        auto presentPass = std::make_unique<PresentPass>("present");
        AddRenderPass( std::move( cleanRTPass ) );
        AddRenderPass( std::move( cleanDSPass ) );
        AddRenderPass( std::move( cleanSMPass ) );
        AddRenderPass( std::move( shadowPass ) );
        AddRenderPass( std::move( lightingPass ));
        AddRenderPass( std::move( skyboxPass ));
        AddRenderPass( std::move( postPass ) );
        AddRenderPass( std::move( presentPass ) );

        SetLinkage("$.mainRT", "cleanRT.buffer");
        SetLinkage("$.depthstencil", "cleanDS.buffer");
        SetLinkage("$.shadowmap", "cleanSM.buffer");

        SetLinkage("cleanSM.buffer", "shadow-cast.shadowmap");

        SetLinkage("shadow-cast.shadowmap", "lighting.shadowmap");
        SetLinkage("cleanRT.buffer", "lighting.renderTarget");
        SetLinkage("cleanDS.buffer", "lighting.depthStencil");

        SetLinkage("lighting.renderTarget", "skybox.renderTarget");
        SetLinkage("lighting.depthStencil","skybox.depthStencil");

        SetLinkage("$.postRT","post-process.postRT");
        SetLinkage("skybox.renderTarget", "post-process.renderTarget");
        SetLinkage("skybox.depthStencil","post-process.depthStencil");
        
        //set to backbuffer.
        SetLinkage("post-process.renderTarget","present.renderTarget");
        SetLinkage("$.backbuffer","present.backbuffer");
        SetLinkage("present.backbuffer","$.backbuffer");

        // print all passes's info
        for (auto & pass : renderPasses)
            pass->ShowInfo();
    }
    
    DXDefaultRenderGraph::~DXDefaultRenderGraph()
    {
        
    }

    
    void DXDefaultRenderGraph::Resize(DirectXGraphics * pGfx)
    {
    }
}