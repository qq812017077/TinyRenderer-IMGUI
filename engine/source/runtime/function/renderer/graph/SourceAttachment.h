#pragma once
#include <string>
#include <memory>
#include "RenderResource.h"
#include "Attachment.h"
#include "SinkAttachment.h"
#include "core/EngineException.h"

namespace TinyEngine::Graph
{
    template<typename RT>
    class SourceAttachment : public Attachment
    {
    public:
        using ResourceType = RT;
        SourceAttachment(std::string name, HandleID& handle): Attachment(name, handle){}
        ~SourceAttachment() = default;
        
        void Bind(Attachment* attachmentPtr) override
        {
            linkedAttach = attachmentPtr;
            if(linkedAttach == nullptr)
            {
                THROW_ENGINE_EXCEPTION("cannot bind sink attachment to source attachment: type mismatch");
            }
        }
        bool IsLinked() const noexcept override {  return linkedAttach != nullptr;  }
    private:
    };
}
