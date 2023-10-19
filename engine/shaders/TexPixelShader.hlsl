#include "Basic.hlsli"

cbuffer MaterialCBuf// : register(b3)
{
    float3 offset;
};

Texture2D _MainTex : register(t1);
SamplerState sampler_MainTex : register(s1);
float4 main(VS_OUTPUT ps_in) : SV_Target
{
	float3 color = _MainTex.Sample(sampler_MainTex, ps_in.tex);
    color += offset;
    return float4(color, 1.0);
}