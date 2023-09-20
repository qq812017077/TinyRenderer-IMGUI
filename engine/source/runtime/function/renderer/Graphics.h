#pragma once
#include <vector>
#include <memory>
#include <string>
#include <queue>
#include "FrameBuffer.h"
class Material;
class VertexShader;
class PixelShader;
class IShaderHelper;
class Graphics
{
    friend class Material;
protected:
    Graphics();

    virtual void DrawAll() = 0;
    
    // Shader Operation
    virtual std::shared_ptr<VertexShader> CreateVertexShader(const std::string& path) = 0;
    virtual std::shared_ptr<PixelShader> CreatePixelShader(const std::string& path) = 0;
    /***
     * @brief Load material to pipeline
     * 1.  Load shader if not loaded
     * 2.  Update material uniform buffer
     * 3.  Bind material info to pipeline 
     *          (   set shaders to pipeline, 
     *              set constant buffer to pipeline, 
     *              set texture to pipeline, 
     *              set sampler to pipeline)
    */
    virtual void LoadMaterial(Material & material);
    static void AddMaterial(Material* pMaterial);
    std::shared_ptr<IShaderHelper> m_pShaderHelper = nullptr;
    
public:
    enum class EBindType
    {
        ToVS,
        ToPS,
        ToAll
    };
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics();
    virtual void ClearBuffer(float red, float green, float blue) noexcept = 0;
    virtual void OnTick(TinyEngine::FrameBuffer * pFrameBuffer);
    virtual void EndFrame() = 0;
    virtual void DrawTestTriangle(float angle=0.0f) = 0;
    
    //Events
    virtual void UpdateRenderSceneViewPort(int pos_x, int pos_y, int width, int height) = 0;
    virtual void OnResize(int width, int height) = 0;

private:
    static std::queue<Material*> m_WaitForLoadMaterials;
};