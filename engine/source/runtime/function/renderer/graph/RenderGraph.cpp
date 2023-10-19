#include <tuple>
#include <algorithm>
#include "RenderGraph.h"
#include "core/EngineException.h"
#include "RenderPass.h"
#include "RenderResource.h"
#include "Attachment.h"


#define CHECK_PASS_EXIST(passName) {if(!ExistPass(passName)) THROW_ENGINE_EXCEPTION("Pass not exist: " + passName);}
#define CHECK_OUTPUT_VALIDATION(passName, outputName) 	\
	if(passName == "$") \
	{	\
		if(globalSources.find(outputName) == globalSources.end()) THROW_ENGINE_EXCEPTION("Global resource not found: " + outputName);\
	}else \
	{	RenderPass * pass = FindPassByName(passName);		\
		if(pass == nullptr) THROW_ENGINE_EXCEPTION("Pass not exist: " + passName); \
		if(!pass->ExistOutput(outputName)) THROW_ENGINE_EXCEPTION("Pass["+passName+"] has not output: " + outputName); }

#define CHECK_INPUT_VALIDATION(passName, inputName) 	\
	if(passName == "$") \
	{	\
		if(globalSinks.find(inputName) == globalSinks.end()) THROW_ENGINE_EXCEPTION("Global resource not found: " + inputName);\
	}else \
	{	RenderPass * pass = FindPassByName(passName);		\
		if(pass == nullptr) THROW_ENGINE_EXCEPTION("Pass not exist: " + passName); \
		if(!pass->ExistInput(inputName)) THROW_ENGINE_EXCEPTION("Pass["+passName+"] has not input: " + inputName); }

namespace TinyEngine::Graph
{

	uint32_t RenderGraph::handle_id_counter = 0;
	template<class Iter>
    void SplitStringIter( const std::string& s,const std::string& delim,Iter out )
    {
        if( delim.empty() )
        {
            *out++ = s;
        }
        else
        {
            size_t a = 0,b = s.find( delim );
            for( ; b != std::string::npos;
                a = b + delim.length(),b = s.find( delim,a ) )
            {
                *out++ = std::move( s.substr( a,b - a ) );
            }
            *out++ = std::move( s.substr( a,s.length() - a ) );
        }
    }

	static std::tuple<std::string, std::string> SplitString( const std::string& s,const std::string& delim )
    {
        std::vector<std::string> strings;
        SplitStringIter( s, delim, std::back_inserter(strings));
        return std::make_tuple(strings[0], strings[1]);
    }

	RenderGraph::RenderGraph(Graphics* pGfx) : pGfx(pGfx)
	{

	}

	RenderGraph::~RenderGraph()
	{

	}
	RenderGraph& RenderGraph::AddRenderPass(std::unique_ptr<RenderPass> pass)
	{
		for(auto & p : renderPasses)
		{
			if(p->GetName() == pass->GetName())
			{
				throw EngineException(__LINE__, __FILE__, "Pass name already exists: " +pass->GetName() );
			}
		}
		renderPassMap[pass->GetName()] = pass.get();
		renderPasses.push_back(std::move(pass));
		renderPasses.back()->Initialize(*this);
		return *this;
	}
	
	
	RenderPass* RenderGraph::FindPassByName( const std::string& name )
	{
		if(renderPassMap.find(name) != renderPassMap.end())
		{
			return renderPassMap[name];
		}
		return nullptr;
	}
	
	void RenderGraph::Execute( Graphics* pGfx)
	{
		for( auto& pass: renderPasses)
		{
			pass->Execute(pGfx, *this);
		}
	}
	
	void RenderGraph::Reset() noexcept
	{
		for( auto& pass : renderPasses )
		{
			pass->Reset();
		}
	}
	void RenderGraph::Clear() noexcept
	{
		renderPasses.clear();
		globalSinks.clear();
		globalSources.clear();
		finalized = false;
	}
	
    void RenderGraph::BindScene(std::shared_ptr<TinyEngine::Scene> pScene)
    {
        for (auto & pass : renderPasses)
            pass->BindScene(pScene);
    }
	void RenderGraph::RefreshResource(Graphics* pGfx)
	{
		for(auto& pair : resourcePools) 
			pair.second->ClearAndAllocate(pGfx);
	}
	/**
	 * @brief 
	 * 
	 * @param source 
	 * @param sink 
	 */
	void RenderGraph::SetLinkage(Source source, Sink sink, bool syncImmediately)
	{
		// firstly, connect source and sink.
		std::string sourcePassName, sourceOutputName;
		std::string sinkPassName, sinkInputName;

		std::tie(sourcePassName, sourceOutputName) = SplitString(source, ".");
		std::tie(sinkPassName, sinkInputName) = SplitString(sink, ".");
		// secondly, sync entire stream state.
		// CHECK_OUTPUT_VALIDATION(sourcePassName, sourceOutputName);
		CheckOutputValidation(sourcePassName, sourceOutputName);
		CheckInputValidation(sinkPassName, sinkInputName);
		// CHECK_INPUT_VALIDATION(sinkPassName, sinkInputName);
		bool globalSource = sourcePassName == "$";
		bool globalSink = sinkPassName == "$";
		if(globalSource && globalSink) THROW_ENGINE_EXCEPTION("Cannot link global resource to global resource");

		if(globalSource)
		{
			// link global resource to pass
			auto sinkAttachment = renderPassMap[sinkPassName]->GetInput(sinkInputName);
			sinkAttachment->Bind(globalSources[sourceOutputName].get());
			// TODO: sync backward to all passes
		}else if(globalSink)
		{
			// TODO: need to 
			auto sourceAttachment = renderPassMap[sourcePassName]->GetOutput(sourceOutputName);
			globalSinks[sinkInputName]->Bind(sourceAttachment);
		}else{
			auto sourceAttachment = renderPassMap[sourcePassName]->GetOutput(sourceOutputName);
			auto sinkAttachment = renderPassMap[sinkPassName]->GetInput(sinkInputName);
			sinkAttachment->Bind(sourceAttachment);
			// TODO: sync backward to all passes
		}
	}
/***********************************************************************************************************/
/*											PROTECTED METHODS										       */
/***********************************************************************************************************/
	bool RenderGraph::ExistPass(std::string passName)
	{
		for( auto & pass : renderPasses)
			if(pass->GetName() == passName) return true;
		return false;
	}

	
	void RenderGraph::CheckInputValidation(std::string passName, std::string inputName)
	{
		if(passName == "$")
		{
			if(globalSinks.find(inputName) == globalSinks.end()) THROW_ENGINE_EXCEPTION("Global resource not found: " + inputName);
		}else
		{
			RenderPass * pass = FindPassByName(passName);
			if(pass == nullptr) THROW_ENGINE_EXCEPTION("Pass not exist: " + passName); 
			if(!pass->ExistInput(inputName)) THROW_ENGINE_EXCEPTION("Pass["+passName+"] has not input: " + inputName); 
		}
	}
	void RenderGraph::CheckOutputValidation(std::string passName, std::string outputName)
	{
		if(passName == "$")
		{
			if(globalSources.find(outputName) == globalSources.end()) THROW_ENGINE_EXCEPTION("Global resource not found: " + outputName);
		}else
		{
			RenderPass * pass = FindPassByName(passName);
			if(pass == nullptr) THROW_ENGINE_EXCEPTION("Pass not exist: " + passName); 
			if(!pass->ExistOutput(outputName)) THROW_ENGINE_EXCEPTION("Pass["+passName+"] has not output: " + outputName);
		}
	}
/***********************************************************************************************************/
/*												PRIVATE METHODS										       */
/***********************************************************************************************************/
	
} // namespace TinyEngine