#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "graphics/BufferResource.h"

class Graphics;
namespace TinyEngine::Graph
{
    class RenderPass;
    enum class ResourceType
    {
        SwapChain,          //SC
        ShaderResource,     //SR
        RenderTarget,       //RT
        DepthStencil,       //DS
        ShadowMap,
        ShadowCubeMap,
        InstanceID,
    };
    
    using HandleID = uint32_t;
    // using ResourceHandle = uint32_t;
    template<typename T>
    class ResourceHandle
    {
        public:
            using RespurceType = T;
            ResourceHandle() = default;
            ~ResourceHandle() = default;

            operator HandleID&() noexcept { return handleID; }
            void operator=(uint32_t handle) noexcept { handleID = handle; }
            bool operator==(const ResourceHandle& other) const
            {
                return handleID == other.handleID;
            }
        public:
            HandleID handleID{UINT_MAX};
    };
    // struct ResourceHandleHash {
    // size_t operator()(const TinyEngine::Graph::ResourceHandle& handle) const {
    //     return handle.handleID;
    //     }
    // };
    // struct ResourceHandleEqual {
    // bool operator()(const TinyEngine::Graph::ResourceHandle& lhs, const TinyEngine::Graph::ResourceHandle& rhs) const {
    //     // 你可以根据 handle 的属性来判断是否相等，例如
    //     return lhs.handleID == rhs.handleID;
    //     }
    // };
    // template<typename Type>
    // struct ResourceHandle
    // {
    //     using RespurceType = Type;
    //     uint32_t handleID{UINT_MAX};
    //     operator uint32_t() const noexcept { return handleID; }
    //     void operator=(uint32_t handle) noexcept { handleID = handle; }
    //     bool operator==(const ResourceHandle& other) const
    //     {
    //         return handleID == other.handleID;
    //     }
    // };

    struct ResourceDesc
    {
        ResourceType type;
        std::string name;
        static ResourceDesc RenderTarget() { return {ResourceType::RenderTarget}; }
        static ResourceDesc DepthStencil() { return {ResourceType::DepthStencil}; }
        static ResourceDesc ShadowMap() { return {ResourceType::ShadowMap}; }
        static ResourceDesc ShadowCubeMap() { return {ResourceType::ShadowCubeMap}; }
        static ResourceDesc ShaderResource() { return {ResourceType::ShaderResource}; }
        static ResourceDesc SwapChain() { return {ResourceType::SwapChain}; }
        static ResourceDesc GOIDRT() { return {ResourceType::InstanceID}; }
        
    };

    class VirtualRenderResource
    {
        public:
            VirtualRenderResource(std::string name, ResourceDesc desc, uint32_t handle): 
                name(name), mDesc(desc), first(nullptr), last(nullptr), handle(handle){}
            virtual ~VirtualRenderResource() = default;
            std::string GetName() const noexcept { return name;}
            virtual void ClearAndAllocate(Graphics* pGfx) = 0;
            virtual ResourceType GetType() const { return mDesc.type; }
            virtual std::shared_ptr<TinyEngine::BufferResource> GetBufferResource() = 0;
            uint32_t handle;
        protected:
            std::string name;
            ResourceDesc mDesc;
            bool instantiated{false};
            std::shared_ptr<RenderPass> first;  // first pass to instance this resource
            std::shared_ptr<RenderPass> last;   // last pass to release this resource

    };

    template<typename T>
    class RenderResource : public VirtualRenderResource
    {
        public:
            using RespurceType = T;
            RenderResource(std::string name, ResourceDesc desc,  HandleID handle): VirtualRenderResource(name, desc, handle){}
            ~RenderResource() override = default;

            void ClearAndAllocate(Graphics* pGfx) override
            {
                pResource.reset();
                pResource = T::Create(pGfx, mDesc);
            }
            void Use(std::shared_ptr<T> resource)
            {
                pResource = resource;
            }
            ResourceType GetType() const override
            {
                return mDesc.type;
            }
            T* GetResource() const noexcept { return pResource.get(); }

            std::shared_ptr<TinyEngine::BufferResource> GetBufferResource() override
            {
                return std::dynamic_pointer_cast<TinyEngine::BufferResource>(pResource);
            }
        protected:
            std::shared_ptr<T> pResource{nullptr};
    };
}