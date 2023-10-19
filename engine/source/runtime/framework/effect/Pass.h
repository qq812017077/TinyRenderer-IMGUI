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
        On
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

	struct DepthStencilDesc
	{
		EDepthMode depthMode{EDepthMode::On};
		EStencilMode stencilMode{EStencilMode::Off};
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

	struct BlendDesc
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

	class CullState : RenderState{
	public:
		ECullMode cullMode{ECullMode::Back};
	};

	class DepthStencilState : RenderState{
	public:
		DepthStencilDesc depthStencilDesc;
	};

	class BlendState : RenderState{
	public:
		BlendDesc blendDesc;
	};

	struct ShaderPass // like Unity's Pass
	{
		ShaderPass() = default;
		ShaderPass(std::string name) noexcept;

		std::string passName;
		// render state, like unity's pass
		ERenderingMode renderingMode{ERenderingMode::Opaque};
		ECullMode cullMode{ECullMode::Back};
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
	
}   // namespace TinyEngine