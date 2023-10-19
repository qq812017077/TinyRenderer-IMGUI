#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "Pass.h"
namespace TinyEngine
{
    struct ShaderPass;
    struct RenderQueue
	{
    public:
        operator uint16_t() const { return m_Value; }

        void operator=(uint16_t value) { m_Value = value; }

		static uint16_t Background;
		static uint16_t Geometry;
		static uint16_t AlphaTest;
		static uint16_t Transparent;
		static uint16_t Overlay;
    
    private:
        uint16_t m_Value;
	};
    
    
	const uint16_t defaultQueuePriority = static_cast<uint16_t>(RenderQueue::Geometry);
    class Effect //类似于Unity中的Shader
    {
    public:
        Effect(std::string effectName);
        ~Effect(){}

        bool IsValid();
        void AddPass(ShaderPass pass);
        ShaderPass& FindPass(std::string name);
        std::string GetName() const { return m_EffectName; }
        size_t GetPassCount() const { return m_Passes.size(); }
        ShaderPass& GetPass(size_t idx) { return m_Passes[idx]; }

        // operator []
        ShaderPass& operator[](size_t idx) { return m_Passes[idx]; }
        ShaderPass& operator[](std::string name) { return FindPass(name); }
        
        void SetRenderingMode(ERenderingMode renderingMode);
        void SetCullMode(ECullMode mode);
        bool CastShadow();
        static std::shared_ptr<Effect> Create(std::string name, ShaderPass pass);
        static std::shared_ptr<Effect> Create(std::string name, std::string vertexShaderPath, std::string pixelShaderPath);

        
		uint16_t queuePriority{defaultQueuePriority};
    private:
        Effect() = default;
    private:
        std::string m_EffectName;
        std::unordered_map<std::string, size_t> m_PassIdxByName;
        std::vector<ShaderPass> m_Passes;
    };
}