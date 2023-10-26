#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Stencil.h"
#include "EffectManager.h"

class VertexShader;
class PixelShader;
class Graphics;
namespace TinyEngine
{
	using namespace TinyEngine::Rendering;
	
	enum class ERenderingMode
	{
		Opaque = 1,
		Cutout = 2,
		Transparent = 3,
		PostProcessing = 4
	};

	
    enum class EDepthMode
    {
        Off,
        On,
		DepthFirst
    };

	enum class ECullMode
	{
		Off = 0,
		Front = 1,
		Back = 2
	};

	enum class ELightMode
	{
		Unlit,
		ForwardBase,
		ForwardAdd,
		Deferred,
		DepthOnly,
		ShadowCaster,
	};

	class RenderState{
		
		virtual void Apply(Graphics* pGfx);

		template<typename T, typename...Params>
		static T&& Create(Params&&...p)
		{
			T state = T(std::forward<Params>(p)...);
			states.push_back(std::move(pState));
			return state;
		}
	};

	struct RenderStateDesc
	{
		virtual int64_t GetHash() const = 0;
	};

	struct DepthStencilDesc : RenderStateDesc
	{
		EDepthMode depthMode{EDepthMode::On};
		EStencilMode stencilMode{EStencilMode::Off};
		
		int64_t GetHash() const override
		{
			int64_t uniqueID = 0;
			
			uniqueID |= (int64_t)(depthMode);
			uniqueID |= (int64_t)(stencilMode) << 8;
			return uniqueID;
			// return std::hash<size_t>()(static_cast<size_t>(depthMode)) ^ std::hash<size_t>()(static_cast<size_t>(stencilMode));
		}
	};
	enum class EBlend
    {
        ZERO					= 1,
        ONE						= 2,
		
		INV_SRC_COLOR			= 4,
        SRC_ALPHA				= 5,
        
		INV_SRC_ALPHA			= 6,
        DEST_ALPHA				= 7,
        INV_DEST_ALPHA			= 8,
        DEST_COLOR				= 9,
        INV_DEST_COLOR			= 10,
        
		SRC_ALPHA_SAT			= 11,
        
		BLEND_FACTOR			= 14,
        INV_BLEND_FACTOR		= 15,
        
		SRC1_COLOR				= 16,
        INV_SRC1_COLOR			= 17,
        SRC1_ALPHA				= 18,
        INV_SRC1_ALPHA			= 19
    };

	enum class EBlendOP
    {
        ADD					= 1,
        SUBTRACT			= 2,
        REV_SUBTRACT		= 3,
        MIN					= 4,
        MAX					= 5
    };
	enum class EBlendWrite
    {
        COLOR_WRITE_ENABLE_RED	= 1,
        COLOR_WRITE_ENABLE_GREEN	= 2,
        COLOR_WRITE_ENABLE_BLUE	= 4,
        COLOR_WRITE_ENABLE_ALPHA	= 8,
        COLOR_WRITE_ENABLE_ALL	= ( ( ( COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN )  | COLOR_WRITE_ENABLE_BLUE )  | COLOR_WRITE_ENABLE_ALPHA ) 
    };

	struct BlendDesc : RenderStateDesc
	{
		bool BlendEnable{false};
		EBlend SrcBlend{EBlend::ZERO};
		EBlend DestBlend{EBlend::ONE};
		EBlendOP BlendOp{EBlendOP::ADD};
		EBlend SrcBlendAlpha{EBlend::ZERO};
		EBlend DestBlendAlpha{EBlend::ONE};
		EBlendOP BlendOpAlpha{EBlendOP::ADD};
		EBlendWrite RenderTargetWriteMask{EBlendWrite::COLOR_WRITE_ENABLE_ALL};
		float * blendFactors{nullptr};

		int64_t GetHash() const override
		{
			int64_t uniqueID = 0;
			uniqueID |= (int64_t)(BlendEnable);
			uniqueID |= (int64_t)(SrcBlend) << 1;
			uniqueID |= (int64_t)(DestBlend) << 6;
			uniqueID |= (int64_t)(BlendOp) << 11;
			uniqueID |= (int64_t)(SrcBlendAlpha) << 14;
			uniqueID |= (int64_t)(DestBlendAlpha) << 19;
			uniqueID |= (int64_t)(BlendOpAlpha) << 24;
			uniqueID |= (int64_t)(RenderTargetWriteMask) << 29;
			
		// 	return std::hash<size_t>()(BlendEnable) ^ 
		// 	std::hash<size_t>()(static_cast<size_t>(SrcBlend)) ^ 
		// 	std::hash<size_t>()(static_cast<size_t>(DestBlend)) ^ 
		// 	std::hash<size_t>()(static_cast<size_t>(BlendOp)) ^ 
		// 	std::hash<size_t>()(static_cast<size_t>(SrcBlendAlpha)) ^ 
		// 	std::hash<size_t>()(static_cast<size_t>(DestBlendAlpha)) ^ 
		// 	std::hash<size_t>()(static_cast<size_t>(BlendOpAlpha)) ^ 
		// 	std::hash<size_t>()(static_cast<size_t>(RenderTargetWriteMask));
			return uniqueID;
		}

		static BlendDesc Transparent()
		{
			BlendDesc desc;
			desc.BlendEnable = true;
			desc.SrcBlend = EBlend::SRC_ALPHA;
			desc.DestBlend = EBlend::INV_SRC_ALPHA;
			desc.BlendOp = EBlendOP::ADD;
			desc.SrcBlendAlpha = EBlend::ZERO;
			desc.DestBlendAlpha = EBlend::ZERO;
			desc.BlendOpAlpha = EBlendOP::ADD;
			desc.RenderTargetWriteMask = EBlendWrite::COLOR_WRITE_ENABLE_ALL;
			return desc;
		}

		static BlendDesc BlendOneOne()
		{
			BlendDesc desc;
			desc.BlendEnable = true;
			desc.SrcBlend = EBlend::ONE;
			desc.DestBlend = EBlend::ONE;
			desc.BlendOp = EBlendOP::ADD;
			desc.SrcBlendAlpha = EBlend::ZERO;
			desc.DestBlendAlpha = EBlend::ZERO;
			desc.BlendOpAlpha = EBlendOP::ADD;
			desc.RenderTargetWriteMask = EBlendWrite::COLOR_WRITE_ENABLE_ALL;
			return desc;
		}
	};

	struct RasterDesc : RenderStateDesc
	{
		// EFillMode FillMode;
		ECullMode cullMode{ECullMode::Back};
		bool FrontCounterClockwise{false};
		int DepthBias{0};
		float DepthBiasClamp{0.0f};
		float SlopeScaledDepthBias{0.0f};
		bool DepthClipEnable{true};
		bool ScissorEnable{false};
		bool MultisampleEnable{false};
		bool AntialiasedLineEnable{false};

		int64_t GetHash() const override
		{
			int64_t uniqueID = 0;
			uniqueID |= (int64_t)(cullMode);
			uniqueID |= (int64_t)(FrontCounterClockwise) << 2;
			uniqueID |= (int64_t)(DepthClipEnable) << 32;
			uniqueID |= (int64_t)(ScissorEnable) << 33;
			uniqueID |= (int64_t)(MultisampleEnable) << 34;
			uniqueID |= (int64_t)(AntialiasedLineEnable) << 35;
			return uniqueID;
		}
		static RasterDesc Default()
		{
			RasterDesc desc;
			desc.cullMode = ECullMode::Back;
			desc.FrontCounterClockwise = false;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0.0f;
			desc.SlopeScaledDepthBias = 0.0f;
			desc.DepthClipEnable = true;
			desc.ScissorEnable = false;
			desc.MultisampleEnable = false;
			desc.AntialiasedLineEnable = false;
			return desc;
		}
	};

	struct ShaderPass // like Unity's Pass
	{
		ShaderPass() = default;
		ShaderPass(std::string name) noexcept;

		std::string passName;
		// render state, like unity's pass
		ERenderingMode renderingMode{ERenderingMode::Opaque};
		RasterDesc rasterDesc;
		DepthStencilDesc depthStencilDesc;
		ELightMode lightMode{ELightMode::Unlit};
		BlendDesc blendDesc;
		// shader
		std::string vsName;
		std::string psName;
		
		static ShaderPass& Default() noexcept
		{
			static ShaderPass pass;
			pass.passName = "DefaultPass";
			pass.vsName = "Default";
			pass.psName = "Default";
			return pass;
		}

		static ShaderPass Get(std::string passName, std::string vertexShaderPath, std::string pixelShaderPath) noexcept
		{
			ShaderPass pass;
			pass.passName = passName;
			pass.vsName = EffectManager::Get().Load(EShaderType::VS, vertexShaderPath);
			pass.psName = EffectManager::Get().Load(EShaderType::PS, pixelShaderPath);
			return pass;
		}
	};

	class RasterState : RenderState{
	public:
		RasterDesc rasterDesc;
	};

	class DepthStencilState : RenderState{
	public:
		DepthStencilDesc depthStencilDesc;
	};

	class BlendState : RenderState{
	public:
		BlendDesc blendDesc;
	};
	
}   // namespace TinyEngine