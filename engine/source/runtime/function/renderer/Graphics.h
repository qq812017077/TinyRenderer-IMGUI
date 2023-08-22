#pragma once
#include <vector>
#include <memory>
#include <string>

class Material;
class VertexShader;
class PixelShader;

class Graphics
{
    friend class Material;
protected:
    Graphics();

    virtual void CreateDevice() = 0;
    virtual void CreateRenderTarget() = 0;
    virtual void ClearupDevice() = 0;
    virtual void ClearupRenderTarget() = 0;

    virtual void DrawAll() = 0;
    virtual void EndFrame() = 0;
    
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
    
    // Constant Buffer Operation
    // virtual void BindFrameCBuffer() = 0;
    // virtual void UpdateObjectCBuffer(Renderer & renderer) = 0;
    // virtual void BindObjectCBuffer() = 0;
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
    virtual void BindImgui() = 0;
    virtual void OnFrameBegin();
    virtual void OnFrameUpdate();
    virtual void OnFrameEnd();
    virtual void DrawTestTriangle(float angle=0.0f) = 0;
    
    //Events
    virtual void OnResize(int width, int height) = 0;
};