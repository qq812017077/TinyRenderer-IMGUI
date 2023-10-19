#pragma once
#include <string>
#include <vector>
#include <memory>
#include "RenderResource.h"
#include "core/EngineException.h"
#include "attachments.h"
class Graphics;
namespace TinyEngine
{
	class Scene;
}
namespace TinyEngine::Graph
{
    class RenderPass;
	class NodeInput;
	class NodeOutput;
	struct ResourceDesc;
	
	class RenderGraph
	{
	public:
		using Source = std::string;
		using Sink = std::string;
	public:
		RenderGraph( Graphics* pGfx );
		virtual ~RenderGraph();

		// pass
		RenderGraph& AddRenderPass(std::unique_ptr<RenderPass> pass);
		RenderPass* FindPassByName( const std::string& name );

		// resource
		template<typename ResourceType>
		ResourceHandle<ResourceType> CreateResource(std::string name, ResourceDesc desc)
		{
			auto resource = std::make_shared<RenderResource<ResourceType>>(name, desc, handle_id_counter++);
			resourcePools[resource->handle] = resource;
			return ResourceHandle<ResourceType>{resource->handle};
		}

		template<typename ResourceType>
		ResourceType* GetBufferResource(ResourceHandle<ResourceType> handle)
		{
			// get actual resouce from handle;
			if(resourcePools.find(handle) == resourcePools.end()) THROW_ENGINE_EXCEPTION("cannot find resource from handle");
			auto result = std::dynamic_pointer_cast<ResourceType>(resourcePools[handle]->GetBufferResource());
			if(result == nullptr) THROW_ENGINE_EXCEPTION("cannot cast resource to target type");
			return result.get();
		}

		template<typename T>
		void AddGlobalSink(ResourceHandle<T>& handle)
		{
			auto resource = resourcePools[handle];
			globalSinks.emplace(resource->GetName(), std::make_unique<SinkAttachment<T>>(resource->GetName(), handle));
		}

		template<typename T>
		void AddGlobalSource(ResourceHandle<T>& handle)
		{
			auto resource = resourcePools[handle];
			globalSources.emplace(resource->GetName(), std::make_unique<SourceAttachment<T>>(resource->GetName(), handle));
		}
		// void AddGlobalSink(std::shared_ptr<VirtualRenderResource> vresource);
		// void AddGlobalSource(std::shared_ptr<VirtualRenderResource> vresource);
		

		virtual void Execute( Graphics* pGfx);
		virtual void Reset() noexcept;
		void Clear() noexcept;
        void BindScene(std::shared_ptr<TinyEngine::Scene> pScene);
		void RefreshResource(Graphics* pGfx);
		// RenderQueuePass& GetRenderQueue( const std::string& passName );
		void SetLinkage(Source source, Sink sink, bool syncImmediate=false);
	protected:
		bool ExistPass(std::string passName);
		void Finalize();
		void CheckInputValidation(std::string passName, std::string inputName);
		void CheckOutputValidation(std::string passName, std::string outputName);
	private:

	protected:
		Graphics* pGfx;
		std::vector<std::unique_ptr<RenderPass>> renderPasses;
		std::unordered_map<std::string, RenderPass*> renderPassMap;

		std::unordered_map<uint32_t, std::shared_ptr<VirtualRenderResource>> resourcePools;
		std::unordered_map<std::string, std::unique_ptr<Attachment>> globalSinks;
		std::unordered_map<std::string, std::unique_ptr<Attachment>> globalSources;
		bool finalized = false;

		static uint32_t handle_id_counter;
	};
} // namespace TinyEngine