#include "Basic.hlsli"

float4 color = float4(1.0, 1.0, 1.0, 1.0);

Texture2D _MainTex : register(t0);
SamplerState sampler_MainTex : register(s0);

float3 get_directional_light(float3 normal, float3 worldPos, DirectionalLight dirLight, float3 texColor)
{
    float3 lightdir = normalize(dirLight.dir);
    float3 light_color = dirLight.color.rgb * texColor;
    float3 vToL = -lightdir;
    float3 vReflect = reflect(lightdir, normal);
    float3 vView = normalize(g_EyePos - worldPos);
    float3 ambient = texColor * float3(0.06, 0.06, 0.06);
    float3 diffuse = light_color * saturate(dot(normal, vToL));
    float3 specular = light_color * pow(saturate(dot(vReflect, vView)), 10);
    float3 _color = ambient + diffuse + specular;
    return _color;
}

float3 get_point_light(float3 normal, float3 worldPos, PointLight pointLight, float3 texColor)
{
    float3 lightdir = worldPos - pointLight.pos;
    float distance = length(lightdir);
    lightdir = normalize(lightdir);
    float x = distance / pointLight.range;
    float attenuation = saturate(1.0f - x * x * x * x);
    
    float3 light_color = pointLight.color.rgb * texColor * attenuation;
    float3 vToL = -lightdir;
    float3 vReflect = reflect(lightdir, normal);
    float3 vView = normalize(g_EyePos - worldPos);
    float3 ambient = texColor * float3(0.06, 0.06, 0.06);
    float3 diffuse = light_color * saturate(dot(normal, vToL));
    float3 specular = light_color * pow(saturate(dot(vReflect, vView)), 10);
    float3 _color = ambient + diffuse + specular;
    return _color;
}

float4 main(VS_OUTPUT ps_in) : SV_Target
{
    // phong shading
    float3 vNormal = normalize(ps_in.normal);
	float4 texColor = _MainTex.Sample(sampler_MainTex, ps_in.tex);
    texColor.rgb = float3(texColor.r * color.r, texColor.g * color.g, texColor.b * color.b);
    // directional light
    float3 dir_color = get_directional_light(vNormal, ps_in.worldPos, g_DirLight, texColor.rgb);
    // point light
    float3 point_color = get_point_light(vNormal, ps_in.worldPos, g_PointLight, texColor.rgb);
    float3 color = dir_color + point_color;
    return float4(color, 1.0);
}