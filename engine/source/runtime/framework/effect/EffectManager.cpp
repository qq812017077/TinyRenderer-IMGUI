#include "EffectManager.h"
#include "Effect.h"
#include "Pass.h"
#include "Graphics.h"
#include <filesystem>
namespace fs = std::filesystem;

namespace TinyEngine
{
    static bool ExistShader(std::string vertexShaderPath, std::string pixelShaderPath)
    {
        if (vertexShaderPath == "" || pixelShaderPath == "")
            return false;
        // check file exist
        if (!fs::exists(vertexShaderPath) || !fs::exists(pixelShaderPath))
            return false;
        return true;
    }

    EffectManager::EffectManager()
    {

    }

    EffectManager::~EffectManager()
    {

    }

    void EffectManager::Initialize()
    {
        // add unlit effect
        auto unlitEffect = Effect::Create("Unlit", "shaders/DefaultVertexShader.hlsl", "shaders/UnlitPixelShader.hlsl");
        auto & pass = (*unlitEffect)["UnlitPass"];
        pass.renderingMode = ERenderingMode::Opaque;
        pass.lightMode = ELightMode::Unlit;
        unlitEffect->queuePriority = RenderQueue::Geometry;

        // add default effect
        auto effect = Effect::Create("Default", "shaders/DefaultVertexShader.hlsl", "shaders/DefaultPixelShader.hlsl");
        auto ShadowPass = ShaderPass::Get("ShadowCastPass", "shaders/effects/ShadowVS.hlsl", "shaders/effects/ShadowPS.hlsl");

        effect->AddPass(ShadowPass);
        auto & defaultPass = (*effect)["DefaultPass"];
        defaultPass.renderingMode = ERenderingMode::Opaque;
        defaultPass.lightMode = ELightMode::ForwardBase;
        effect->queuePriority = RenderQueue::Geometry;
        (*effect)["ShadowCastPass"].lightMode = ELightMode::ShadowCaster;


        // add cutout effect
        auto cutoutEffect = Effect::Create("DefaultCutout", "shaders/DefaultVertexShader.hlsl", "shaders/DefaultCutoutPixelShader.hlsl");
        auto & cutoutPass = (*cutoutEffect)["DefaultCutoutPass"];
        cutoutPass.renderingMode = ERenderingMode::Cutout;
        cutoutPass.lightMode = ELightMode::ForwardBase;
        cutoutPass.rasterDesc.cullMode = ECullMode::Off;
        cutoutEffect->queuePriority = RenderQueue::AlphaTest;
        
        // add transparent effect
        auto transparentEffect = Effect::Create("DefaultTransparent", "shaders/DefaultVertexShader.hlsl", "shaders/DefaultTransparentPixelShader.hlsl");
        transparentEffect->queuePriority = RenderQueue::Transparent;
        (*transparentEffect)[0].blendDesc = BlendDesc::Transparent();
        transparentEffect->SetRenderingMode(ERenderingMode::Transparent);
        transparentEffect->SetCullMode(ECullMode::Off);
        
        // add outline effect
        auto outlineEffect = Effect::Create("Outline", "shaders/DefaultVertexShader.hlsl", "shaders/effects/OutlinePS.hlsl");
        outlineEffect->FindPass("OutlinePass").renderingMode = ERenderingMode::PostProcessing;

        // add fullscreen effect
        auto BlurEffect = Effect::Create("Blur", "shaders/effects/FullScreenVS.hlsl", "shaders/effects/BlurPS.hlsl");
        BlurEffect->queuePriority = RenderQueue::Overlay;
        
        auto BlitEffect = Effect::Create("Blit", "shaders/effects/FullScreenVS.hlsl", "shaders/effects/BlitPS.hlsl");
        BlitEffect->queuePriority = RenderQueue::Overlay;

        // skybox effect
        auto skyboxEffect = Effect::Create("SkyBox", "shaders/SkyBoxVS.hlsl", "shaders/SkyBoxPS.hlsl");
        

        // add error effect
        auto errorEffect = Effect::Create("Error", "shaders/ErrorVertexShader.hlsl", "shaders/ErrorPixelShader.hlsl");
        errorEffect->FindPass("ErrorPass").renderingMode = ERenderingMode::PostProcessing;


    }

    std::shared_ptr<Effect> EffectManager::FindEffect(const std::string& name)
    {
        if(m_EffectsByName.find(name) != m_EffectsByName.end())
            return m_EffectsByName[name];
        else
            return m_EffectsByName["Error"];
    }
    
    ShaderPass& EffectManager::FindPass(const std::string passPath)
    {
        // get effect name from passPath
        auto pos = passPath.find('/');
        auto effectName = passPath.substr(0, pos);
        auto passName = passPath.substr(pos+1);
        auto pEffect = FindEffect(effectName);
        return (*pEffect)[passName];
    }

    
    std::shared_ptr<Effect> EffectManager::FindDefaultEffect(ERenderingMode mode)
    {
        switch (mode)
        {
        case ERenderingMode::Opaque:
            return FindEffect("Default");
        case ERenderingMode::Cutout:
            return FindEffect("DefaultCutout");
        case ERenderingMode::Transparent:
            return FindEffect("DefaultTransparent");
        }

        return FindEffect("Error");
    }

    
    void EffectManager::AddEffect(const std::string name, std::shared_ptr<Effect> pEffect)
    {
        m_EffectsByName.emplace(name, pEffect);
    }
    
    
    void EffectManager::LoadShaders(Graphics * pGfx)
    {
        //遍历 
        for(size_t i = 0, imax = m_WaitForLoad.size(); i < imax; ++i)
        {
            auto& shaderDesc = m_WaitForLoad[i];
            switch (shaderDesc.type)
            {
            case EShaderType::VS:
                {
                    if(m_VertexShaders.find(shaderDesc.name) != m_VertexShaders.end())
                        break;
                    auto vsPtr = pGfx->CreateVertexShader(shaderDesc.path);
                    m_VertexShaders[shaderDesc.name] = vsPtr;
                }
                break;
            case EShaderType::PS:
                {
                    if(m_PixelShaders.find(shaderDesc.name) != m_PixelShaders.end())
                        break;
                    auto psPtr = pGfx->CreatePixelShader(shaderDesc.path);
                    m_PixelShaders[shaderDesc.name] = psPtr;
                }
                break;
            default:
                break;
            }
        }

        m_WaitForLoad.clear();
    }
        
    
    std::string EffectManager::Load(EShaderType shaderType, std::string shaderPath)
    {
        std::string shaderName = shaderPath;
        
        // build map: shaderName -> shaderPath
        nameBypath[shaderName] = shaderPath;
        m_WaitForLoad.emplace_back(
            ShaderDesc{
                shaderName,
                shaderType,
                shaderPath
            }
        );
        return shaderName;
    }

    VertexShader * EffectManager::VSFindShader(std::string name)
    {
        auto it = m_VertexShaders.find(name);
        if (it != m_VertexShaders.end())
        {
            return it->second.get();
        }
        return nullptr;
    }
    PixelShader* EffectManager::PSFindShader(std::string name)
    {
        auto it = m_PixelShaders.find(name);
        if (it != m_PixelShaders.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    bool EffectManager::IsValidShader(const std::string& shadername)
    {
        if(nameBypath.find(shadername) != nameBypath.end())
        {
            auto shaderPath = nameBypath[shadername];
            return fs::exists(shaderPath);
        }
        return true;
    }

}