
#include "../Basic.hlsli"

SHADOW_VS_OUTPUT main(float3 pos : Position)
{
	SHADOW_VS_OUTPUT output;
	// float3 pos = v_in.pos;
	output.pos = mul(float4(pos, 1.0), g_World);
	output.pos = mul(output.pos, g_LightVP);
	return output;
}