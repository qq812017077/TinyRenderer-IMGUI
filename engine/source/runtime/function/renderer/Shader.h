#pragma once
#include <string>
#include <vector>
#include <string>
#include "UniformBuffer.h"
#include "Mesh.h"
#include "UniformVar.h"
class ShaderBase {
protected:

    ShaderBase() = default;
    ~ShaderBase() = default;

    std::vector<UniformVar> uniformVars;
public :
    virtual std::vector<UniformVar> getUniformVars() const {return uniformVars;};
};

class VertexShader : public ShaderBase {
protected:
    VertexShader() = default;
    ~VertexShader() = default;

public:
    virtual void updateMaterialUniformBuffer(UniformBuffer uniformBuffer) = 0;
    virtual void bindToPipeline() = 0;
    virtual void SetInputLayout(Mesh& mesh) = 0;
};


class PixelShader : public ShaderBase {
protected:
    PixelShader() = default;
    ~PixelShader() = default;
    
    
public:
    virtual void updateMaterialUniformBuffer(UniformBuffer uniformBuffer) = 0;
    virtual void bindToPipeline() = 0;
};

