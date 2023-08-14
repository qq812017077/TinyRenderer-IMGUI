#pragma once
#include "core/Singleton.h"
#include <vector>
#include "EngineMath.h"
#include <memory>

class Camera;

class UniformBuffer
{
public:
    UniformBuffer() = default;
    ~UniformBuffer() = default;
    UniformBuffer(UniformBuffer const& other) = default;
    UniformBuffer& operator=(UniformBuffer& other) noexcept;
    
    void * operator&();
    unsigned int bytesize();
    void* GetAddress();
    void Clear();

    // Add data to buffer
    void AddData(Matrix4x4 data);
    void AddData(void* data, unsigned int size);
    void AddData(float data);
    void AddData(int data);
    void AddData(unsigned int data);
    void AddData(bool data);
private:
    std::vector<float> buffers;
};

class FrameUniformBufferManager : public Singleton<FrameUniformBufferManager>
{
public:
    FrameUniformBufferManager() = default;
    ~FrameUniformBufferManager() = default;
    FrameUniformBufferManager(FrameUniformBufferManager const& other) = delete;
    FrameUniformBufferManager& operator=(FrameUniformBufferManager& other) noexcept = delete;

    static void UpdateFrameBuffer();
    static UniformBuffer& GetFrameBuffer();
    static void ClearFrameBuffer();
    static void BindCamera(Camera* camera);

private:
    UniformBuffer frameBuffer;
    Camera* pCurCamera = nullptr;
};