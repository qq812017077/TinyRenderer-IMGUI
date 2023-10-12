#pragma once
#include <string>
#include <vector>
#include <string>
#include "Mesh.h"
#include "ShaderDesc.h"
#include <memory>

struct Color;
class Material;
class ShaderBase {
protected:

    ShaderBase() = default;
    ~ShaderBase() = default;

    ShaderDesc shaderDescInfo;
public :
    virtual ShaderDesc& GetShaderDesc();
    virtual int GetTexCount() const;

    std::shared_ptr<ShaderResource> CreateShaderResource();
    virtual void LoadMaterialResource(Material * pMat) = 0;
    // virtual void UpdateTexture(Material* pMat) = 0;

    // virtual SamplerInfo GetSamplerInfoByName(const char * name, int slot);

    // virtual bool SetBool(const char * name, bool value);
    // virtual bool SetInteger(const char * name, int value);
    // virtual bool SetFloat(const char * name, float value);
    // virtual bool SetVariable(const char * name, void * data, unsigned int bytesize);
    // virtual bool SetColor(const char * name, Color& color);
    // virtual bool SetMatrix(const char * name, Matrix4x4& matrix);
    // virtual bool SetVector(const char * name, Vector4& vec);
    // virtual bool SetVector(const char * name, Vector3& vec);
    // virtual bool SetTexture(const char * name, std::shared_ptr<Texture> pTex, std::string& samplerName);

    
    // virtual std::shared_ptr<ICBufferVariable> GetVariableByName(const char * name);
    // virtual std::shared_ptr<ICBufferVariable> GetVariableBySlot(int slot);

    // virtual std::shared_ptr<ITextureVariable> GetTextureByName(const char * name);
    // virtual std::shared_ptr<ITextureVariable> GetTextureBySlot(int slot);
};

// 定义
// using VertexShader = ShaderBase;
// using PixelShader = ShaderBase;
class VertexShader : public ShaderBase {
protected:
    VertexShader() = default;
    ~VertexShader() = default;
};


class PixelShader : public ShaderBase {
protected:
    PixelShader() = default;
    ~PixelShader() = default;
};


class IShaderHelper {
public:
    // virtual std::shared_ptr<ICBufferVariable> GetConstantBufferVariable(const char * varName) = 0;
    virtual bool SetGlobalVariable(const char * varName, void * data, unsigned int bytesize) = 0;
    virtual bool SetGlobalMatrix(const char * varName, Matrix4x4& matrix) = 0;
    virtual bool SetGlobalVector(const char * varName, Vector3& vec) = 0;
    virtual bool SetVector(const char * varName, Vector4& vec) = 0;
};
