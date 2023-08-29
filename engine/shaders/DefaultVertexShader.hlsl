#include "Basic.hlsli"

// cbuffer ObjectCBuf : register(b2)
// {
// 	matrix g_World;
// };

VS_OUTPUT main(VS_INPUT v_in)
{
	VS_OUTPUT output;
	// float3 pos = v_in.pos;
	output.pos = mul(float4(v_in.pos, 1.0), g_World);
	output.worldPos = output.pos;
	output.normal = mul(float4(v_in.normal, 1.0), transpose(g_WorldInv)).xyz;
	output.pos = mul(output.pos, g_View);
	output.pos = mul(output.pos, g_Proj);
	output.color = v_in.color;
	output.tex = v_in.tex;
	return output;
}