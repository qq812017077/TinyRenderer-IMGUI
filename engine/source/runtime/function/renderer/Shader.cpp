#include "Shader.h"

int ShaderBase::GetTexCount() const
{
    return static_cast<int>(shaderDescInfo.textures.size());
}

ShaderDesc ShaderBase::GetShaderDesc() const 
{
    return shaderDescInfo;
}

CBufInfo ShaderBase::GetConstantBufferInfoBySlot(int slot)
{
    auto &constantBufs = shaderDescInfo.constantBufs;
    auto &cBufIndexBySlot = shaderDescInfo.cBufIndexBySlot;
    if (cBufIndexBySlot.find(slot) == cBufIndexBySlot.end())
    {
        return constantBufs[0];
    }
    return constantBufs[cBufIndexBySlot[slot]];
}
CBufInfo ShaderBase::GetConstantBufferInfoByName(const std::string& name)
{
    auto &constantBufs = shaderDescInfo.constantBufs;
    auto &cBufIndexByName = shaderDescInfo.cBufIndexByName;
    if (cBufIndexByName.find(name) == cBufIndexByName.end())
    {
        return constantBufs[0];
    }
    return constantBufs[cBufIndexByName[name]];
}


TextureInfo ShaderBase::GetTexInfoByIndex(int index) const
{
    return shaderDescInfo.textures[index];
}
TextureInfo ShaderBase::GetTextureInfoBySlot(int slot)
{
    auto &textures = shaderDescInfo.textures;
    auto &texIndexBufBySlot = shaderDescInfo.texIndexBufBySlot;
    if (texIndexBufBySlot.find(slot) == texIndexBufBySlot.end())
    {
        return textures[0];
    }
    return textures[texIndexBufBySlot[slot]];
}


TextureInfo ShaderBase::GetTextureInfoByName(const std::string& name)
{
    auto &textures = shaderDescInfo.textures;
    auto &texIndexBufByName = shaderDescInfo.texIndexBufByName;
    if(texIndexBufByName.find(name) == texIndexBufByName.end())
    {
        return textures[0];
    }
    return textures[texIndexBufByName[name]];
}


SamplerInfo ShaderBase::GetSamplerInfoBySlot(int slot)
{
    auto &samplers = shaderDescInfo.samplers;
    auto &samplerIndexBufBySlot = shaderDescInfo.samplerIndexBufBySlot;
    if (samplerIndexBufBySlot.find(slot) == samplerIndexBufBySlot.end())
    {
        return samplers[0];
    }
    return samplers[samplerIndexBufBySlot[slot]];
}

SamplerInfo ShaderBase::GetSamplerInfoByName(const std::string& name, int slot)
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