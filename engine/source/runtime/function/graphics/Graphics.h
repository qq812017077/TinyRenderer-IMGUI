#pragma once
#include <vector>
#include <memory>
#include <string>
#include <queue>
#include "FrameBuffer.h"
#include "effect/EffectManager.h"
#include "effect/EffectPass.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "graph/RenderGraph.h"

class Material;
class Renderer;
class VertexShader;
class PixelShader;
class IShaderHelper;
class Mesh;
namespace TinyEngine
{
    class RenderState;
    class RenderEntity;
}

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

    enum class EDrawMode
    {
        TriangleList,
        TriangleStrip,
        LineList,
        LineStrip,
        PointList
    };

    struct ViewPort
    {
        int pos_x;
        int pos_y;
        unsigned int width;
        unsigned int height;
    };
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics();
    virtual void ClearBuffer(float red, float green, float blue) noexcept = 0;
    virtual void OnTick(TinyEngine::FrameBuffer * pFrameBuffer);
    virtual void EndFrame() = 0;

    virtual std::shared_ptr<TinyEngine::RenderTarget> CreateRenderTarget() = 0;
    virtual std::shared_ptr<TinyEngine::DepthStencil> CreateDepthStencil() = 0;
    virtual void BindRenderTarget(TinyEngine::RenderTarget* pRenderTarget, TinyEngine::DepthStencil * pDepthStencil) = 0;
    virtual void BindDefaultRenderTarget() = 0;
    // virtual void DrawTestTriangle(float angle=0.0f) = 0;
    
    virtual void SetViewport(ViewPort viewPort) = 0;

    virtual void UnbindAllResource() = 0;
    virtual void ApplyState(TinyEngine::RenderState * pState) = 0;
    virtual void ApplyPass(TinyEngine::ShaderPass & pass) = 0;
    virtual void Draw(TinyEngine::RenderEntity* entity, EDrawMode mode = EDrawMode::TriangleList) = 0;
    virtual void ApplyPassToRenderList(TinyEngine::ShaderPass & pass, std::vector<Renderer*> & renderers) = 0;
    virtual void ApplyPassToRenderTarget(TinyEngine::ShaderPass & pass, TinyEngine::RenderTarget * pRenderers) = 0;
    virtual void ApplyPassToMesh(TinyEngine::ShaderPass & pass, Mesh * pMesh, EDrawMode mode = EDrawMode::TriangleList) = 0;
    
    //Events
    virtual void UpdateRenderSceneViewPort(float pos_x, float pos_y, float width, float height) = 0;
    virtual void OnResize(int width, int height) = 0;
    int GetWidth() const noexcept { return m_width; }
    int GetHeight() const noexcept { return m_height; }
    virtual size_t PickGuidOfGameObject(float u, float v) = 0;
protected:
    TinyEngine::MousePickEffectPass m_mousepicking_pass;

    std::unique_ptr<TinyEngine::Graph::RenderGraph> m_pRenderGraph = nullptr;
    std::shared_ptr<Mesh> quad = nullptr;
    int m_width;
    int m_height;
private:

};