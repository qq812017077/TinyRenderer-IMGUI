#include "Basic.hlsli"
#include "Math.hlsli"
#include "PBR.hlsli"


Texture2D _MainTex : register(t1);
SamplerState sampler_MainTex : register(s1);


float4 albedo = float4(1.0, 1.0, 1.0, 1.0);

float4 main(VS_OUTPUT ps_in) : SV_Target
{
	float4 sColor = _MainTex.Sample(sampler_MainTex, ps_in.tex);
    clip(sColor.a < 0.1f? -1 : 1);
    float3 texColor = sColor.rgb * albedo.rgb;

    // directional light
    float3 dir_color = get_directional_light(ps_in, g_DirLight, texColor);
    // point light
    float3 point_color = get_point_light(ps_in, g_PointLight, texColor);
    float3 color = dir_color + point_color;

    float3 ambient = texColor.rgb * float3(0.03, 0.03, 0.03);
    return float4(color, sColor.a);
}