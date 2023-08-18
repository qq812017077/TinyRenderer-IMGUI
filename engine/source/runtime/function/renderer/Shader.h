#pragma once
#include <string>
#include <vector>
#include <string>
#include "UniformBuffer.h"
#include "Mesh.h"
#include "ShaderDesc.h"

class ShaderBase {
protected:

    ShaderBase() = default;
    ~ShaderBase() = default;

    ShaderDesc shaderDescInfo;
public :
    virtual ShaderDesc GetShaderDesc() const ;
    virtual int GetTexCount() const;
    
    virtual CBufInfo GetConstantBufferInfoBySlot(int slot);
    virtual CBufInfo GetConstantBufferInfoByName(const std::string& name);
    
    virtual TextureInfo GetTexInfoByIndex(int index) const;
    virtual TextureInfo GetTextureInfoBySlot(int slot);
    virtual TextureInfo GetTextureInfoByName(const std::string& name);
    
    virtual SamplerInfo GetSamplerInfoBySlot(int slot);
    virtual SamplerInfo GetSamplerInfoByName(const std::string& name, int slot);
};

class VertexShader : public ShaderBase {
protected:
    VertexShader() = default;
    ~VertexShader() = default;

public:
    virtual void SetInputLayout(Mesh& mesh) = 0;
};


class PixelShader : public ShaderBase {
protected:
    PixelShader() = default;
    ~PixelShader() = default;
    
public:
};

