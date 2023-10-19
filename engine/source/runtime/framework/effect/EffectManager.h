#pragma once
#include "core/Singleton.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

class VertexShader;
class PixelShader;
class Graphics;

namespace TinyEngine
{
    class Effect;
    struct ShaderPass;
    enum class ERenderingMode;
    enum class EShaderType
    {
        VS = 1,
        PS = 2
    };

    struct ShaderDesc
    {
        std::string name;
        EShaderType type;
        std::string path;
    };

    
    class EffectManager : public Singleton<EffectManager>
    {
    public:
        EffectManager();
        ~EffectManager();

        void Initialize();
        std::shared_ptr<Effect> FindEffect(const std::string& name);
        ShaderPass& FindPass(const std::string passPath);
        std::shared_ptr<Effect> FindDefaultEffect(ERenderingMode mode);
        void AddEffect(const std::string name, std::shared_ptr<Effect> pEffect);
        void LoadShaders(Graphics * pGfx);
        std::string Load(EShaderType shaderType, std::string shaderPath);

        template<typename T>
        T* VSFindShader(std::string name)
        {
            auto it = m_VertexShaders.find(name);
            if (it != m_VertexShaders.end())
            {
                return reinterpret_cast<T*>(it->second.get());
            }
            return nullptr;
        }

        template<typename T>
        T* PSFindShader(std::string name)
        {
            auto it = m_PixelShaders.find(name);
            if (it != m_PixelShaders.end())
            {
                return reinterpret_cast<T*>(it->second.get());
            }
            return nullptr;
        }

        VertexShader * VSFindShader(std::string name);
        PixelShader* PSFindShader(std::string name);
        
        bool IsValidShader(const std::string& name);
    private:
        std::unordered_map<std::string, std::shared_ptr<Effect>> m_EffectsByName;
        std::unordered_map<std::string, std::shared_ptr<VertexShader>> m_VertexShaders;		
        // std::unordered_map<std::string, std::shared_ptr<HullShaderInfo>> m_HullShaders;			
        // std::unordered_map<std::string, std::shared_ptr<DomainShaderInfo>> m_DomainShaders;		
        // std::unordered_map<std::string, std::shared_ptr<GeometryShaderInfo>> m_GeometryShaders;
        std::unordered_map<std::string, std::shared_ptr<PixelShader>> m_PixelShaders;		
        // std::unordered_map<std::string, std::shared_ptr<ComputeShaderInfo>> m_ComputeShaders;
        
        std::unordered_map<std::string, std::string> nameBypath;
        std::vector<ShaderDesc> m_WaitForLoad;
    };
}