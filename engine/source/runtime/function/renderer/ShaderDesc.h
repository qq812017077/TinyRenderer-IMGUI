#pragma once
#include <string>
#include <map>
#include <vector>
struct UniformVar
{
    std::string name;
    // std::string type;
    unsigned int size;
    // int offset;
    // int arraySize;
    // int arrayStride;
    // int matrixStride;
    UniformVar(std::string name, unsigned int size)
    {
        this->name = name;
        this->size = size;
    }
};

struct CBufInfo
{
    int slot;
    std::string BufName;
    std::vector<UniformVar> vars;
};

struct TextureInfo
{
    int slot;
    std::string name;
};

struct SamplerInfo
{
    int slot;
    std::string name;
};

struct inputLayoutInfo
{
    std::string semanticName;
    unsigned int size;
    unsigned int offset;
};

class ShaderDesc
{
    friend class ShaderBase;
public:
    void AddConstantBufferInfo(CBufInfo cbInfo)
    {
        constantBufs.push_back(cbInfo);
        cBufIndexBySlot[cbInfo.slot] = static_cast<int>(constantBufs.size() - 1);
        cBufIndexByName[cbInfo.BufName] = static_cast<int>(constantBufs.size() - 1);
    }
    void AddTextureInfo(TextureInfo texInfo)
    {
        textures.push_back(texInfo);
        texIndexBufBySlot[texInfo.slot] = static_cast<int>(textures.size() - 1);
        texIndexBufByName[texInfo.name] = static_cast<int>(textures.size() - 1);
    }
    void AddSamplerInfo(SamplerInfo samplerInfo)
    {
        samplers.push_back(samplerInfo);
        samplerIndexBufBySlot[samplerInfo.slot] = static_cast<int>(samplers.size() - 1);
        samplerIndexBufByName[samplerInfo.name] = static_cast<int>(samplers.size() - 1);
    }

    int GetConstantBufferCInfoount() const
    {
        return static_cast<int>(constantBufs.size());
    }

    int GetTextureInfoCount() const
    {
        return static_cast<int>(textures.size());
    }

    int GetSamplerInfoCount() const
    {
        return static_cast<int>(samplers.size());
    }
private:
    std::vector<CBufInfo> constantBufs;
    std::vector<TextureInfo> textures;
    std::vector<SamplerInfo> samplers;
    
    std::map<int, int> cBufIndexBySlot;
    std::map<std::string, int> cBufIndexByName;

    std::map<int, int> texIndexBufBySlot;
    std::map<std::string, int> texIndexBufByName;

    std::map<int, int> samplerIndexBufBySlot;
    std::map<std::string, int> samplerIndexBufByName;

};