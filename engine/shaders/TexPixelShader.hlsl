#include "Basic.hlsli"

Texture2D _MainTex : register(t0);
SamplerState sampler_MainTex : register(s0);
float4 main(VS_OUTPUT ps_in) : SV_Target
{
	float3 color = _MainTex.Sample(sampler_MainTex, ps_in.tex);
    return float4(color, 1.0);
}