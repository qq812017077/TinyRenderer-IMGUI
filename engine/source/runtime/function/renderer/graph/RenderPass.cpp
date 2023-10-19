#include "RenderPass.h"
#include "core/EngineException.h"
#include "Attachment.h"
#include <iostream>

namespace TinyEngine::Graph
{

    const std::vector<std::unique_ptr<Attachment>>& RenderPass::GetInputs() const noexcept 
    { 
        return inputs; 
    }

    const std::vector<std::unique_ptr<Attachment>>& RenderPass::GetOutputs() const noexcept 
    { 
        return outputs; 
    }

    Attachment* RenderPass::GetInput(std::string sinkName) const noexcept 
    { 
        return inputMap.at(sinkName); 
    }
    Attachment* RenderPass::GetOutput(std::string sourceName) const noexcept 
    { 
        return outputMap.at(sourceName); 
    }

    bool RenderPass::ExistInput(std::string inputName) const noexcept 
    { 
        return inputMap.find(inputName) != inputMap.end(); 
    }
    bool RenderPass::ExistOutput(std::string outputName) const noexcept 
    { 
        return outputMap.find(outputName) != outputMap.end(); 
    }

    
    void RenderPass::ShowInfo()
    {
        // show pass name
        // std::cout << "------------------ ["+ name +"] ------------------" << std::endl;
        // // show inputs
        // std::cout << "inputs:" << std::endl;
        // for (auto& input : inputs)
        // {
        //     std::cout << "   name:" << input->GetName() << "  handle:" << input->GetResourceHandle().handleID << std::endl;
        // }
        // // show outputs
        // std::cout << "outputs:" << std::endl;
        // for (auto& output : outputs)
        // {
        //     std::cout << "   name:" << output->GetName() << "  handle:" << output->GetResourceHandle().handleID << std::endl;
        // }
        // std::cout << "-------------------------------------------------" << std::endl; 
    }
/********************************************************************************************/
/*                                      Protected                                          */
/********************************************************************************************/
    RenderPass& RenderPass::RegisterSink(std::unique_ptr<Attachment> inputPtr)
    {
        auto inputName = inputPtr->GetName();
        // check if input name is already registered
        for (auto& input : inputs)
        {
            if (input->GetName() == inputName)
            {
                THROW_ENGINE_EXCEPTION("input has exist, cannot register:" + inputName);
            }
        }
        inputMap[inputName] = inputPtr.get();
        inputs.push_back(std::move(inputPtr));
        return *this;
    }
    RenderPass& RenderPass::RegisterSource(std::unique_ptr<Attachment> outputPtr)
    {
        auto outputName = outputPtr->GetName();
        // check if output name is already registered
        for (auto& output : outputs)
        {
            if (output->GetName() == outputName)
            {
                THROW_ENGINE_EXCEPTION("output has exist, cannot register:" + outputName);
            }
        }
        outputMap[outputName] = outputPtr.get();
        outputs.push_back(std::move(outputPtr));
        return *this;
    }
    
    RenderPass& RenderPass::SetExecuteCallback(std::function<void(Graphics *)> callback) { 
        executeCallback = callback; return *this;
    }
}
