#include "Shader.h"
#include "Color.h"


static float colordata[4];
int ShaderBase::GetTexCount() const
{
    return static_cast<int>(shaderDescInfo.textures.size());
}

ShaderDesc& ShaderBase::GetShaderDesc() 
{
    return shaderDescInfo;
}

std::shared_ptr<ShaderResource> ShaderBase::CreateShaderResource()
{
    auto pShaderResource = std::make_shared<ShaderResource>();
    
    for(auto & cbuffer : shaderDescInfo.cbuffers)
    {
        pShaderResource->CBufferBySlot.emplace(
            cbuffer.bindPoint, std::make_unique<CBufferData>(cbuffer.bufferName, cbuffer.bindPoint, cbuffer.bufSize)
        );
        for(auto& var : cbuffer.variables)
        {
            pShaderResource->variables.emplace(var.varName, std::make_shared<CBufferVariable>(var.startOffset, var.size, 
            pShaderResource->CBufferBySlot[cbuffer.bindPoint].get()));
        }
    }
    
    for(auto & tex : shaderDescInfo.textures)
        pShaderResource->textures.emplace(tex.textureName, std::make_shared<TextureVariable>(tex.bindPoint, tex.textureName));
    
    int i{0};
    for(auto & sampler : shaderDescInfo.samplers)
    {
        pShaderResource->samplers.emplace_back(SamplerInfo(sampler.samplerName, sampler.bindPoint));
        pShaderResource->samplerIndexBufBySlot[sampler.bindPoint] = i;
        pShaderResource->samplerIndexBufByName[sampler.samplerName] = i;
        i++;
    }
    return pShaderResource;
}
