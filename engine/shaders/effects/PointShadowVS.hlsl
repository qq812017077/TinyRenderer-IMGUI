
#include "../Basic.hlsli"

SHADOW_VS_OUTPUT main(float3 pos : Position)
{
	SHADOW_VS_OUTPUT output;
	output.pos = mul(float4(pos, 1.0), g_World);
	output.viewPos = mul(output.pos, g_LightView);
	output.pos = mul(output.pos, g_LightViewProj);
	return output;
}