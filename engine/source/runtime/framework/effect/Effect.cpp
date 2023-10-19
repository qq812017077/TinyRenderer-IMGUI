#include "Effect.h"
#include "EffectManager.h"
#include "EffectPass.h"
#include "core/EngineException.h"

namespace TinyEngine
{
    
    uint16_t RenderQueue::Background = 1000;
    uint16_t RenderQueue::Geometry = 2000;
    uint16_t RenderQueue::AlphaTest = 2450;
    uint16_t RenderQueue::Transparent = 3000;
    uint16_t RenderQueue::Overlay = 4000;

    Effect::Effect(std::string effectName)
    {
        m_EffectName = effectName;
    }
    
    bool Effect::IsValid()
    {
        if(m_Passes.size() == 0)
            return false;
        for(int i = 0 ; i < m_Passes.size(); ++i)
        {
            if(!EffectManager::Get().IsValidShader(m_Passes[i].vsName)) return false;
            if(!EffectManager::Get().IsValidShader(m_Passes[i].psName)) return false;
        }
        return true;
    }
    
    void Effect::AddPass(ShaderPass pass)
    {
        m_Passes.emplace_back(pass);
        m_PassIdxByName.emplace(pass.passName, m_Passes.size() - 1);
    }
    
    ShaderPass& Effect::FindPass(std::string name)
    {
        if(m_PassIdxByName.find(name) != m_PassIdxByName.end())
            return m_Passes[m_PassIdxByName[name]];
        else
            THROW_ENGINE_EXCEPTION("Pass not found: " + name);
    }

    void Effect::SetRenderingMode(ERenderingMode renderingMode)
    {
        for(int i = 0; i < m_Passes.size(); ++i)
        {
            m_Passes[i].renderingMode = renderingMode;
        }
    }
    void Effect::SetCullMode(ECullMode mode)
    {
        for(int i = 0; i < m_Passes.size(); ++i)
        {
            m_Passes[i].cullMode = mode;
        }
    }

    bool Effect::CastShadow()
    {
        for(int i = 0; i < m_Passes.size(); ++i)
        {
            if(m_Passes[i].lightMode == ELightMode::ShadowCaster)
                return true;
        }
        return false;
    }

/***************************************************************************************/
    std::shared_ptr<Effect> Effect::Create(std::string effectName, ShaderPass pass)
    {
        auto pEffect = std::make_shared<Effect>(effectName);
        pEffect->AddPass(pass);

        EffectManager::Get().AddEffect(effectName, pEffect);
        return pEffect;
    }

    std::shared_ptr<Effect> Effect::Create(std::string effectName, std::string vertexShaderPath, std::string pixelShaderPath)
    {
        ShaderPass pass;
        pass.passName = effectName + "Pass";
        pass.vsName = EffectManager::Get().Load(EShaderType::VS, vertexShaderPath);
        pass.psName = EffectManager::Get().Load(EShaderType::PS, pixelShaderPath);
        
        return Create(effectName, pass);
    }
}