#include "Basic.hlsli"
#include "Math.hlsli"
// #include "Lambert.hlsli"
#include "PBR.hlsli"

float4 albedo = float4(1.0, 1.0, 1.0, 1.0);

Texture2D _MainTex : register(t1);
SamplerState sampler_MainTex : register(s1);

float4 main(VS_OUTPUT ps_in) : SV_Target
{
    ps_in.normal = normalize(ps_in.normal);
	float4 texColor = _MainTex.Sample(sampler_MainTex, ps_in.tex);
    texColor.rgb = texColor.rgb * albedo.rgb;
    
    float3 ambient = texColor.rgb * get_ambient(ps_in, texColor.rgb);
    // directional light
    float3 dir_color = get_directional_light(ps_in, g_DirLight, texColor.rgb);
    // point light
    float3 point_color = get_point_light(ps_in, g_PointLight, texColor.rgb);
    
    float3 color = dir_color + point_color;
    return float4(ambient + color, 1.0);
}