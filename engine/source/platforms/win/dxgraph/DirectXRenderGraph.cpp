#include "DirectXRenderGraph.h"
#include "DirectXGraphics.h"
#include "graph/RenderPass.h"
#include "graph/Attachment.h"
#include "graph/RenderResource.h"
#include "passlib.h"
#include "dxgraphics/DirectXRenderTarget.h"
#include "dxgraphics/DirectXDepthStencil.h"

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
        shadowCubeHandle = CreateResource<CubeRenderTexture>("shadowcubemap", ResourceDesc::ShadowCubeMap());
        goIdRTHandle = CreateResource<DirectXRenderTarget>("idRT", ResourceDesc::GOIDRT());
        goIdDSHandle = CreateResource<DirectXDepthStencil>("idDS", ResourceDesc::DepthStencil());

        AddGlobalSource(backBufferHandle);
        AddGlobalSource(mainRTHandle);
        AddGlobalSource(effectRTHandle);
        AddGlobalSource(goIdRTHandle);
        AddGlobalSource(goIdDSHandle);
        AddGlobalSource(depthStencilHandle);
        AddGlobalSource(shadowMapHandle);
        AddGlobalSource(shadowCubeHandle);
        AddGlobalSink(backBufferHandle);
        
        auto cleanRTPass = std::make_unique<BufferPass>("cleanRT");
        auto cleanDSPass = std::make_unique<BufferPass>("cleanDS");
        auto cleanSMPass = std::make_unique<BufferPass>("cleanSM");
        auto cleanSMCubePass = std::make_unique<BufferPass>("cleanSMCube");
        auto shadowPass = std::make_unique<ShadowPass>("shadow-cast");
        auto lightingPass = std::make_unique<ShadingPass>("lighting");
        auto pickPass = std::make_unique<PickPass>("pick");
        auto skyboxPass = std::make_unique<SkyBoxPass>("skybox");
        auto postPass = std::make_unique<PostProcessPass>("post-process");
        auto hubPass = std::make_unique<HubPass>("hub");
        auto presentPass = std::make_unique<PresentPass>("present");
        AddRenderPass( std::move( cleanRTPass ) );
        AddRenderPass( std::move( cleanDSPass ) );
        AddRenderPass( std::move( cleanSMPass ) );
        AddRenderPass( std::move( cleanSMCubePass ) );
        AddRenderPass( std::move( shadowPass ) );
        AddRenderPass( std::move( lightingPass ));
        AddRenderPass( std::move( pickPass ));
        AddRenderPass( std::move( skyboxPass ));
        AddRenderPass( std::move( postPass ) );
        AddRenderPass( std::move( hubPass ) );
        AddRenderPass( std::move( presentPass ) );

        SetLinkage("$.mainRT", "cleanRT.buffer");
        SetLinkage("$.depthstencil", "cleanDS.buffer");
        SetLinkage("$.shadowmap", "cleanSM.buffer");
        SetLinkage("$.shadowcubemap", "cleanSMCube.buffer");

        SetLinkage("cleanSM.buffer", "shadow-cast.shadowmap");
        SetLinkage("cleanSMCube.buffer", "shadow-cast.shadowcubemap");

        SetLinkage("shadow-cast.shadowmap", "lighting.shadowmap");
        SetLinkage("shadow-cast.shadowcubemap", "lighting.shadowcubemap");
        SetLinkage("cleanRT.buffer", "lighting.renderTarget");
        SetLinkage("cleanDS.buffer", "lighting.depthStencil");

        SetLinkage("$.idRT","pick.renderTarget");
        SetLinkage("$.backbuffer","pick.backbuffer");
        SetLinkage("$.idDS","pick.depthStencil");

        SetLinkage("lighting.renderTarget", "skybox.renderTarget");
        SetLinkage("lighting.depthStencil","skybox.depthStencil");

        SetLinkage("$.postRT","post-process.postRT");
        SetLinkage("skybox.renderTarget", "post-process.renderTarget");
        SetLinkage("skybox.depthStencil","post-process.depthStencil");
        
        SetLinkage("post-process.renderTarget","hub.renderTarget");
        SetLinkage("post-process.depthStencil","hub.depthStencil");
        
        //set to backbuffer.
        SetLinkage("hub.renderTarget","present.renderTarget");
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