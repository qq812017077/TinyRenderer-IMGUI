#pragma once
#include <vector>
#include <memory>
#include <string>
#include <queue>
#include "FrameBuffer.h"
#include "effect/EffectManager.h"
#include "effect/EffectPass.h"
class Material;
class Renderer;
class VertexShader;
class PixelShader;
class IShaderHelper;
class Graphics
{
    friend class Material;
    friend class TinyEngine::EffectManager;
protected:
    Graphics();

    
    // Shader Operation
    virtual std::shared_ptr<VertexShader> CreateVertexShader(const std::string& path) = 0;
    virtual std::shared_ptr<PixelShader> CreatePixelShader(const std::string& path) = 0;
    
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
    virtual void DrawAll(TinyEngine::FrameBuffer * pFrameBuffer) = 0;
    virtual void Apply(TinyEngine::ShaderPass & pass, std::vector<Renderer*> & renderers) = 0;
    
    //Events
    virtual void UpdateRenderSceneViewPort(int pos_x, int pos_y, int width, int height) = 0;
    virtual void OnResize(int width, int height) = 0;

protected:
    // we need four pass, include lighting, shadow, post process, mouse picking pass
    // TinyEngine::PShadowEffectPass       m_shadow_pass;
    // TinyEngine::PShadingEffectPass m_shading_pass;
    // TinyEngine::PPostProcessEffectPass  m_postprocess_pass;
    TinyEngine::MousePickEffectPass    m_mousepicking_pass;

private:
    static std::queue<Material*> m_WaitForLoadMaterials;

};