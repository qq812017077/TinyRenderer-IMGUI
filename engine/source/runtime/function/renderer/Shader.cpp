#include "Shader.h"
#include "Color.h"
int ShaderBase::GetTexCount() const
{
    return static_cast<int>(shaderDescInfo.textures.size());
}

ShaderDesc& ShaderBase::GetShaderDesc() 
{
    return shaderDescInfo;
}
static float colordata[4];

bool ShaderBase::SetBool(const char * name, bool value)
{
    if(shaderDescInfo.variables.find(name) == shaderDescInfo.variables.end()) return false;
    auto & variable = shaderDescInfo.variables[name];
    variable->SetBool(value);
    return true;
}
bool ShaderBase::SetInteger(const char * name, int value)
{
    if(shaderDescInfo.variables.find(name) == shaderDescInfo.variables.end()) return false;
    auto & variable = shaderDescInfo.variables[name];
    variable->SetInt(value);
    return true;
}

bool ShaderBase::SetFloat(const char * name, float value)
{
    if(shaderDescInfo.variables.find(name) == shaderDescInfo.variables.end()) return false;
    auto & variable = shaderDescInfo.variables[name];
    variable->SetFloat(value);
    return true;
}

bool ShaderBase::SetVariable(const char * name, void * data, unsigned int bytesize)
{
    if(shaderDescInfo.variables.find(name) == shaderDescInfo.variables.end()) return false;
    auto & variable = shaderDescInfo.variables[name];
    variable->SetRaw(data, 0, bytesize);
    return true;
}

bool ShaderBase::SetColor(const char * name, Color& color)
{
    colordata[0] = color.GetR() / 255.0f;
    colordata[1] = color.GetG() / 255.0f;
    colordata[2] = color.GetB() / 255.0f;
    colordata[3] = color.GetA() / 255.0f;
    if(shaderDescInfo.variables.find(name) == shaderDescInfo.variables.end()) return false;
    auto & variable = shaderDescInfo.variables[name];
    variable->SetFloatVector(4, colordata);
    return true;
}

bool ShaderBase::SetMatrix(const char * name, Matrix4x4& matrix)
{
    if(shaderDescInfo.variables.find(name) == shaderDescInfo.variables.end()) return false;
    auto & variable = shaderDescInfo.variables[name];
    variable->SetFloatMatrix(4, 4, matrix.GetData());
    return true;
}

bool ShaderBase::SetVector(const char * name, Vector4& vec)
{
    if(shaderDescInfo.variables.find(name) == shaderDescInfo.variables.end()) return false;
    auto & variable = shaderDescInfo.variables[name];
    variable->SetFloatVector(4, reinterpret_cast<float*>(&vec));
    return true;
}

bool ShaderBase::SetVector(const char * name, Vector3& vec)
{
    if(shaderDescInfo.variables.find(name) == shaderDescInfo.variables.end()) return false;
    auto & variable = shaderDescInfo.variables[name];
    variable->SetFloatVector(3, reinterpret_cast<float*>(&vec));
    return true;
}



SamplerInfo ShaderBase::GetSamplerInfoByName(const char * name, int slot)
{
    auto &samplers = shaderDescInfo.samplers;
    auto &samplerIndexBufByName = shaderDescInfo.samplerIndexBufByName;
    if (samplerIndexBufByName.find(name) == samplerIndexBufByName.end())
    {
        auto &samplerIndexBufBySlot = shaderDescInfo.samplerIndexBufBySlot;
        // try to find by slot
        if (samplerIndexBufBySlot.find(slot) == samplerIndexBufBySlot.end())
        {
            return samplers[0];
        }
        return samplers[samplerIndexBufBySlot[slot]];
    }
    return samplers[samplerIndexBufByName[name]];
}

std::shared_ptr<ICBufferVariable> ShaderBase::GetVariableByName(const char * name)
{
    if(shaderDescInfo.variables.find(name) == shaderDescInfo.variables.end())
    {
        return nullptr;
    }
    return shaderDescInfo.variables[name];
}

std::shared_ptr<ICBufferVariable> ShaderBase::GetVariableBySlot(int slot)
{
    for(auto &var : shaderDescInfo.variables)
    {
        if(var.second->GetSlot() == slot)
        {
            return var.second;
        }
    }
    return nullptr;
}

std::shared_ptr<ITextureVariable> ShaderBase::GetTextureByName(const char * name)
{
    if(shaderDescInfo.textures.find(name) == shaderDescInfo.textures.end())
    {
        return nullptr;
    }
    return shaderDescInfo.textures[name];
}

std::shared_ptr<ITextureVariable> ShaderBase::GetTextureBySlot(int slot)
{
    for(auto &tex : shaderDescInfo.textures)
    {
        if(tex.second->GetSlot() == slot)
        {
            return tex.second;
        }
    }
    return nullptr;
}