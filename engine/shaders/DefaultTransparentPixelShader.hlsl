#include "Basic.hlsli"
#include "Math.hlsli"
// #include "PBR.hlsli"
#include "Lambert.hlsli"


float4 albedo = float4(1.0, 1.0, 1.0, 1.0);

Texture2D _MainTex : register(t1);
SamplerState sampler_MainTex : register(s1);
float4 main(VS_OUTPUT ps_in) : SV_Target
{
    // phong shading
	float4 texColor = _MainTex.Sample(sampler_MainTex, ps_in.tex);
    texColor.rgb = float3(texColor.r * albedo.r, texColor.g * albedo.g, texColor.b * albedo.b);
    // directional light
    float3 dir_color = get_directional_light(ps_in, g_DirLight, texColor.rgb);
    // point light
    float3 point_color = get_point_light(ps_in, g_PointLight, texColor.rgb);
    float3 color = dir_color + point_color;
    return float4(color, texColor.a);
}