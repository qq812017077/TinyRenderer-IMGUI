#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Stencil.h"


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
		float * blendFactors{nullptr};
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
	};

	
}   // namespace TinyEngine