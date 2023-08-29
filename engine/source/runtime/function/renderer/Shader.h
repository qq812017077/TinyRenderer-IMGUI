#pragma once
#include <string>
#include <vector>
#include <string>
#include "Mesh.h"
#include "ShaderDesc.h"

struct Color;
class ShaderBase {
protected:

    ShaderBase() = default;
    ~ShaderBase() = default;

    ShaderDesc shaderDescInfo;
public :
    virtual ShaderDesc& GetShaderDesc();
    virtual int GetTexCount() const;
    
    virtual SamplerInfo GetSamplerInfoByName(const char * name, int slot);
    
    virtual bool SetBool(const char * name, bool value);
    virtual bool SetInteger(const char * name, int value);
    virtual bool SetFloat(const char * name, float value);
    virtual bool SetVariable(const char * name, void * data, unsigned int bytesize);
    virtual bool SetColor(const char * name, Color& color);
    virtual bool SetMatrix(const char * name, Matrix4x4& matrix);
    virtual bool SetVector(const char * name, Vector4& vec);
    virtual bool SetVector(const char * name, Vector3& vec);

    virtual std::shared_ptr<ICBufferVariable> GetVariableByName(const char * name);
    virtual std::shared_ptr<ICBufferVariable> GetVariableBySlot(int slot);

    virtual std::shared_ptr<ITextureVariable> GetTextureByName(const char * name);
    virtual std::shared_ptr<ITextureVariable> GetTextureBySlot(int slot);
    virtual void UpdateConstantBuffer() = 0;
};

class VertexShader : public ShaderBase {
protected:
    VertexShader() = default;
    ~VertexShader() = default;

public:
    virtual void SetInputLayout() = 0;
};


class PixelShader : public ShaderBase {
protected:
    PixelShader() = default;
    ~PixelShader() = default;
    
public:
};


class IShaderHelper {
public:
    // virtual std::shared_ptr<ICBufferVariable> GetConstantBufferVariable(const char * varName) = 0;
    virtual bool SetGlobalVariable(const char * varName, void * data, unsigned int bytesize) = 0;
    virtual bool SetGlobalMatrix(const char * varName, Matrix4x4& matrix) = 0;
    virtual bool SetGlobalVector(const char * varName, Vector3& vec) = 0;
    virtual bool SetVector(const char * varName, Vector4& vec) = 0;
};
