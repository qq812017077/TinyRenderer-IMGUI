#include "UniformBuffer.h"
#include "Camera.h"

/****************************************************************************************/
/*                                      UniformBuffer                                    */
/****************************************************************************************/
unsigned int UniformBuffer::bytesize()
{
    return static_cast<unsigned int>(buffers.size() * sizeof(float));
}

void * UniformBuffer::GetAddress()
{
    return buffers.data();
}

void * UniformBuffer::operator&()
{
    return buffers.data();
}

void UniformBuffer::Clear()
{
    buffers.clear();
}

void UniformBuffer::AddData(Matrix4x4 data)
{
    for (unsigned int i = 0; i < 16; i++)
    {
        buffers.push_back(data[i/4][i%4]);
    }
}

void UniformBuffer::AddData(void * data, unsigned int size)
{
    float* p = (float*)data;
    for (unsigned int i = 0; i < size / sizeof(float); i++)
    {
        buffers.push_back(p[i]);
    }
}

void UniformBuffer::AddData(float data)
{
    buffers.push_back(data);
}

void UniformBuffer::AddData(int data)
{
    buffers.push_back((float)data);
}

void UniformBuffer::AddData(unsigned int data)
{
    buffers.push_back((float)data);
}

void UniformBuffer::AddData(bool data)
{
    buffers.push_back((float)data);
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer& other) noexcept
{
    buffers = other.buffers;
    return *this;
}


/****************************************************************************************/
/*                                FrameUniformBufferManager                              */
/****************************************************************************************/


void FrameUniformBufferManager::UpdateFrameBuffer()
{
    auto &manager = FrameUniformBufferManager::Get();
    if(manager.pCurCamera == nullptr)
    {
        throw std::exception("No camera is binded to FrameUniformBufferManager");
    }
    manager.frameBuffer.AddData(manager.pCurCamera->GetViewMatrix());
    manager.frameBuffer.AddData(manager.pCurCamera->GetProjectionMatrix());
}

UniformBuffer& FrameUniformBufferManager::GetFrameBuffer()
{
    return FrameUniformBufferManager::Get().frameBuffer;
}

void FrameUniformBufferManager::ClearFrameBuffer()
{
    FrameUniformBufferManager::Get().frameBuffer.Clear();
}

void FrameUniformBufferManager::BindCamera(Camera* camera)
{
    FrameUniformBufferManager::Get().pCurCamera = camera;
}