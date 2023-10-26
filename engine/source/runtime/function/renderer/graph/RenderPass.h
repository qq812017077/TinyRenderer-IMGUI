#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include "attachments.h"
class Graphics;
namespace TinyEngine
{
    class Scene;
}
namespace TinyEngine::Graph
{
    class RenderGraph;

    class RenderPass
    {
    public:
        RenderPass(std::string name): name(name) {}
        virtual ~RenderPass() = default;
        
        virtual std::string GetName() const noexcept { return name; }

        virtual void Initialize(Graphics* pGfx, RenderGraph & graph) {}
        
        virtual void Execute(Graphics *pGfx, RenderGraph& graph) { if (executeCallback) executeCallback(pGfx); }
        virtual void Reset() noexcept {}
        virtual void BindScene(std::shared_ptr<TinyEngine::Scene> pScene) noexcept { this->pScene = pScene; }
        
        void ShowInfo();

        const std::vector<std::unique_ptr<Attachment>>& GetInputs() const noexcept;
        const std::vector<std::unique_ptr<Attachment>>& GetOutputs() const noexcept;
        Attachment* GetInput(std::string sinkName) const noexcept;
        Attachment* GetOutput(std::string sourceName) const noexcept;
        bool ExistInput(std::string inputName) const noexcept;
        bool ExistOutput(std::string outputName) const noexcept;
    protected:
        RenderPass& RegisterSink(std::unique_ptr<Attachment> inputPtr);
        RenderPass& RegisterSource(std::unique_ptr<Attachment> outputPtr);
        RenderPass& SetExecuteCallback(std::function<void(Graphics *)> callback);

        std::string name;
        std::vector<std::unique_ptr<Attachment>> inputs;
        std::vector<std::unique_ptr<Attachment>> outputs;

        std::unordered_map<std::string, Attachment*> inputMap;
        std::unordered_map<std::string, Attachment*> outputMap;
        std::function<void(Graphics *)> executeCallback;

        std::shared_ptr<TinyEngine::Scene> pScene{nullptr};
    };

    
    
}
