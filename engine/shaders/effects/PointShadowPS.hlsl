#include "../Basic.hlsli"

float4 main(SHADOW_VS_OUTPUT ps_in) : SV_Target
{
    // float bias = 0.0005 + (ps_in.pos.z - ps_in.viewPos.w / ps_in.pos.w);
	float bias = 0.0005;
    return saturate(length(ps_in.viewPos.xyz) / g_PointLight.range + bias);
}