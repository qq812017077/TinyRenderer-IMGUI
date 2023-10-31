#pragma once
#include <string>
#include <vector>
#include <string>
#include "Mesh.h"
#include "ShaderDesc.h"
#include <memory>
#include <unordered_map>

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
    virtual bool SetGlobalUINT(const char * varName, size_t uintVal) = 0;
};
