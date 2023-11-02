#pragma once
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "effect/Effect.h"
#include "Texture.h"
#include "effect/Stencil.h"
#include <variant>
#include "core/math/Matrix.h"
#include "Color.h"

struct Vector3;
struct Vector4;
class Graphics;
class ShaderBase;
class Renderer;
struct ShaderResource;
using TinyEngine::Effect;
using TinyEngine::RenderQueue;
using TinyEngine::ERenderingMode;
// using TinyEngine::Rendering::EStencilMode;
// using TinyEngine::ECullMode;

class Material
{
    friend class Graphics;
    friend class RenderQueueManager;
private:
    struct TextureInfo
    {
        std::shared_ptr<Texture> pTexture;
        std::string samplerName;
    };
public:
    enum class EVarType
    {
        Integer,
        Float,
        _Color,
        Matrix4x4,
        Texture
    };

    template<typename T>
    struct VarMap { using Type = T; };
    #define VAR_TYPE(type, enumType) template<> struct VarMap<type> { using Type = type; static constexpr EVarType TypeEnum = EVarType::enumType; }
    VAR_TYPE(int, Integer);
    VAR_TYPE(float, Float);
    VAR_TYPE(Color, _Color);
    VAR_TYPE(Matrix4x4, Matrix4x4);
    VAR_TYPE(TextureInfo, Texture);
    #undef VAR_TYPE

    class Variable
    {
    public:
        template<typename T>
        operator T&()
        {
            if(type != VarMap<T>::TypeEnum) { throw std::runtime_error("Type mismatch"); }
            dirty = false;
            return std::get<T>(variable);
        }

        template<typename T>
        operator T() const
        {
            return const_cast<Variable&>(*this).operator T&();
        }

        template<typename T>
        void operator=(const T& value)
        {
            variable = value;
            type = VarMap<T>::TypeEnum;
            dirty = true;
        }
        bool IsDirty() const { return dirty; }
        void MakeDirty() { dirty = true; }
        EVarType GetType() const { return type; }
    private:
        using Var = std::variant<int, float, TextureInfo, Matrix4x4, Color>;
        Var variable;
        EVarType type;
        bool dirty;
    };
    
private:
    Material() = delete;
    // Material(std::string vertexShaderPath, std::string pixelShaderPath, std::string materialName, size_t mat_id);
    Material(std::shared_ptr<Effect> pEffect, std::string materialName, size_t mat_id);
public:
    Material(const Material&);
    ~Material();
    
    void SetupEffect(std::string effectName);
    std::shared_ptr<Effect> GetEffect() const { return pEffect; }

    size_t GetInstanceID() const;

    std::string GetName() const { return materialName; }
    // Renderer 
    void Bind(Renderer * pRenderer);
    void UnBind(Renderer * pRenderer);
    void SetRenderQueuePriority(RenderQueue renderQueue) { SetRenderQueuePriority((uint16_t)renderQueue);}
    void SetRenderQueuePriority(uint16_t priority);
    uint16_t GetRenderQueuePriority() const {return queuePriority;}

    // resource operation
    std::shared_ptr<ShaderResource> GetUpdatedShaderResourcePtr(ShaderBase* shader);
    std::unordered_map<std::string, Variable>& GetVariableMap() { return variableMap; }
    // uniform buffer operation
    void SetInteger(const char * name, int value);
    void SetFloat(const char * name, float value);
    void SetColor(const char * name, Color value);
    void SetMatrix(const char * name, Matrix4x4& value);
    void SetTexture(const char * name, std::shared_ptr<Texture> pTexture, const char * samplerName = nullptr);
    bool GetInteger(const char * name, int* result);
    bool GetFloat(const char * name, float* result);
    bool Material::GetMatrix4x4(const char * name, float* result, int size);
    
    Texture* GetTexturePtr(const char * name);
    
    std::string GetSamplerNameByTexName(std::string& texName) const;
    static int GetRefCount(std::shared_ptr<Material> pMaterial);
    static std::shared_ptr<Material> CreateInstance(std::shared_ptr<Material> pMaterial);
    static std::shared_ptr<Material> CreateDefault(std::string materialName = "New Material", ERenderingMode renderingMode=ERenderingMode::Opaque);
    static std::shared_ptr<Material> Create(std::string effectName, std::string materialName = "");
    static std::shared_ptr<Material> Create(std::shared_ptr<Effect> pEffect, std::string materialName = "");
    static std::shared_ptr<Material> GetDefaultMaterialPtr(ERenderingMode renderingMode=ERenderingMode::Opaque);
private:
    

    size_t instanceId{0};
    std::string materialName;
    uint16_t queuePriority;

    std::shared_ptr<Effect> pEffect;
    std::unordered_map<std::string, Variable> variableMap;
    
    std::unordered_map<Renderer*, bool> rendererRefCountMap;
    std::map<ShaderBase *, std::shared_ptr<ShaderResource>> shaderResourceMap;

    static std::shared_ptr<Material> pDefaultStandardMaterial;
    static std::shared_ptr<Material> pDefaultCutoutMaterial;
    static std::shared_ptr<Material> pDefaultTransparentMaterial;
};
