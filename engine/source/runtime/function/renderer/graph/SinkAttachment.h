#pragma once
#include <string>
#include <memory>
#include "RenderResource.h"
#include "Attachment.h"
#include "SourceAttachment.h"
#include "core/EngineException.h"

namespace TinyEngine::Graph
{

    template<typename RT>
    class SinkAttachment : public Attachment
    {
    public:
        using ResourceType = RT;
        SinkAttachment(std::string name, HandleID& handle): Attachment(name, handle){}
        ~SinkAttachment() = default;

        void Bind(Attachment* source) override
        {
            linkedAttach = source;
            if(linkedAttach == nullptr)
            {
                THROW_ENGINE_EXCEPTION("cannot bind sink attachment to source attachment: type mismatch");
            }
            handle = linkedAttach->GetResourceHandle();
        }
        
        bool IsLinked() const noexcept override{  return linkedAttach != nullptr;  }
        
    private:
        
    };

}
