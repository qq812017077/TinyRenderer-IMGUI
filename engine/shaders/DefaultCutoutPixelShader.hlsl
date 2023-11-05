#include "Basic.hlsli"
#include "Math.hlsli"
#include "PBR.hlsli"


float4 albedo = float4(1.0, 1.0, 1.0, 1.0);
Texture2D _MainTex : register(t1);
SamplerState sampler_MainTex : register(s1);


float4 main(VS_OUTPUT ps_in) : SV_Target
{
    ps_in.normal = normalize(ps_in.normal);
	float4 sColor = _MainTex.Sample(sampler_MainTex, ps_in.tex);
    clip(sColor.a < 0.1f? -1 : 1);
    float3 texColor = sColor.rgb * albedo.rgb;
    
    float3 ambient = texColor.rgb * get_ambient(ps_in, texColor);
    // float3 ambient = texColor.rgb * float3(0.03, 0.03, 0.03);

    // directional light
    float3 dir_color = get_directional_light(ps_in, g_DirLight, texColor);
    // point light
    float3 point_color = get_point_light(ps_in, g_PointLight, texColor);
    float3 color = dir_color + point_color;

    return float4(color, sColor.a);
}