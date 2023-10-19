#pragma once
#include <string>
#include <memory>
#include "RenderResource.h"
#include "core/EngineException.h"

namespace TinyEngine::Graph
{
    class Attachment
    {
    public:
        Attachment(std::string name, HandleID& handle): name(name), handle(handle) {}
        ~Attachment() = default;

        virtual void Bind(Attachment* attachmentPtr) = 0;
        virtual bool IsLinked() const noexcept = 0;
        const std::string& GetName() const noexcept { return name; }
        HandleID& GetResourceHandle() const noexcept { return handle; }
    protected:
        std::string name;
        Attachment* linkedAttach{nullptr};
        HandleID& handle;
    };
}
