#include "Basic.hlsli"

VS_OUTPUT main(VS_INPUT v_in)
{
	VS_OUTPUT output;
	output.pos = mul(float4(v_in.pos, 1.0), g_World);
	output.pos = mul(output.pos, g_ViewProj);
	return output;
}