
#ifndef __PBR_H__
#define __PBR_H__

#include "Math.hlsli"
#include "Basic.hlsli"
#include "Shadow.hlsli"

#define DielectricMetal 0.04

float metallic = 0.0;
float smoothness = 0.0; // 1 - roughness

float3 PrefilterEnvMap(float roughness, float3 reflect)
{
    float3 prefilteredColor = _PrefilteredEnvMap.SampleLevel(sampler_PrefilteredEnvMap, reflect, roughness * 5.0).rgb;
    return prefilteredColor;
}

float2 IntegrateBRDF(float NoV , float roughness)
{
    return _BRDFLutMap.Sample(sampler_BRDFLutMap, float2(NoV, roughness)).rg;
}

float3 ApproximateSpecularIBL( float3 SpecularColor , float Roughness, float3 N, float3 V )
{
    float NoV = saturate( dot( N, V ) );
    float3 R = 2 * dot( V, N ) * N - V;
    float3 PrefilteredColor = PrefilterEnvMap( Roughness, R );
    float2 EnvBRDF = IntegrateBRDF(NoV, Roughness);
    return PrefilteredColor * ( SpecularColor * EnvBRDF.x + EnvBRDF.y );
}

/**
    F0: base reflectance at normal incidence
*/
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float smoothness)
{
    return F0 + (max(float3(smoothness, smoothness, smoothness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   
/**
    NDF: normal distribution function
*/
float DistributionGGX(float3 N, float3 H, float roughness)
{   
    roughness = clamp(roughness, 0.01, 0.99);
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

/**
    Geometry function
*/
float GeometrySchlickGGX(float NdotV, float roughness)
{
    roughness = clamp(roughness, 0.01, 0.99);
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}

/**
    Geometry function
*/
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float3 get_ambient(VS_OUTPUT ps_in, float3 albedo)
{
    // return float3(0.0, 0.0, 0.0);
    float3 normal = ps_in.normal;
    float3 viewdir = normalize(g_EyePos - ps_in.worldPos);
    float3 reflectDir = reflect(-viewdir, normal);
    float n_dot_v = max(dot(normal, viewdir), 0.0);
    float3 dielectric = float3(DielectricMetal, DielectricMetal, DielectricMetal);
    float3 F0 = lerp(dielectric, albedo, float3(metallic,metallic,metallic));

    float3 F = fresnelSchlickRoughness(n_dot_v, F0, smoothness);
    
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    // diffuse
    float3 irradiance = _IrradianceMap.Sample(sampler_IrradianceMap, normal);
    float3 diffuse    = irradiance * albedo;

    
    // specular
    float3 prefilteredColor = PrefilterEnvMap(1-smoothness, reflectDir);
    float2 envBRDF = IntegrateBRDF(n_dot_v, 1 - smoothness);
    float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
    // float3 specular = float3(0.0, 0.0, 0.0);
    
    float3 ambient    = kD * diffuse + specular;
    return ambient;
}


float3 get_directional_light(VS_OUTPUT ps_in, DirectionalLight dirLight, float3 albedo)
{
    float3 normal = ps_in.normal;
    float3 worldPos = ps_in.worldPos;
    float3 lightdir = -normalize(dirLight.dir);
    float3 viewdir = normalize(g_EyePos - worldPos);
    float3 halfdir = normalize(lightdir + viewdir);

    float3 dielectric = float3(DielectricMetal, DielectricMetal, DielectricMetal);
    float3 fresnel = fresnelSchlick(max(dot(normal, halfdir), 0.0f), lerp(dielectric, albedo.rgb, float3(metallic,metallic,metallic)));
    float NDF = DistributionGGX(normal, halfdir, 1 - smoothness);
    float G   = GeometrySmith(normal, viewdir, lightdir, 1- smoothness);

    float3 numerator    = NDF * G * fresnel;
    float denominator = 4.0 * max(dot(normal, viewdir), 0.0) * max(dot(normal, lightdir), 0.0)  + 0.0001;
    float3 specular     = numerator / denominator;

    float3 kS = fresnel;            // specular
    float3 kD = float3(1.0, 1.0, 1.0) - kS;   // diffuse
    kD *= 1.0 - metallic;

    float filterSize = 1.0 / float(SAMPLE_SIZE); //SAMPLE_SIZE should be size of Shadow Map,so it equals to  '1 / textureSize(shadowMap, 0)'
    float visibility = DIR_PCF(ps_in.lightPos / ps_in.lightPos.w, filterSize,  0.005);
    // float visibility = useShadowMap(ps_in.lightPos / ps_in.lightPos.w, 0.005);
    // float visibility = PCSS(ps_in.lightPos / ps_in.lightPos.w, filterSize,  0.005);
    return (kD * albedo.rgb / PI + specular) * dirLight.color.rgb * visibility;
}

float3 get_point_light(VS_OUTPUT ps_in, PointLight pointLight, float3 albedo)
{
    float3 normal = ps_in.normal;
    float3 worldPos = ps_in.worldPos;

    float3 lightdir = pointLight.pos - worldPos;
    float distance = length(lightdir);
    lightdir = normalize(lightdir);
    
    float x = distance / pointLight.range;
    float attenuation = saturate(1.0f - x * x * x * x);
    if(attenuation == 0.0f) return float3(0.0f, 0.0f, 0.0f);
    
    float3 viewdir = normalize(g_EyePos - worldPos);
    float3 halfdir = normalize(lightdir + viewdir);
    // fresnel term

    float3 dielectric = float3(DielectricMetal, DielectricMetal, DielectricMetal);
    float3 fresnel = fresnelSchlick(max(dot(normal, halfdir), 0.0f), lerp(dielectric, albedo.rgb, float3(metallic,metallic,metallic)));
    float NDF = DistributionGGX(normal, halfdir, 1 - smoothness);
    float G   = GeometrySmith(normal, viewdir, lightdir, 1- smoothness);

    float3 numerator    = NDF * G * fresnel;
    float denominator = 4.0 * max(dot(normal, viewdir), 0.0) * max(dot(normal, lightdir), 0.0)  + 0.0001;
    float3 specular     = numerator / denominator;

    float3 kS = fresnel;            // specular
    float3 kD = float3(1.0, 1.0, 1.0) - kS;   // diffuse
    kD *= 1.0 - metallic;

    float filterSize = 1.0 / float(SAMPLE_SIZE); //SAMPLE_SIZE should be size of Shadow Map,so it equals to  '1 / textureSize(shadowMap, 0)'
    float visibility = POINT_SHADOW(worldPos - pointLight.pos, pointLight.range,  0.005);

    float3 light_color = pointLight.color.rgb * attenuation;
    return (kD * albedo.rgb / PI + specular) * light_color * visibility;
}


half3 EnvBRDFApprox( half3 SpecularColor, half Roughness, half NoV )
{
    // [ Lazarov 2013, "Getting More Physical in Call of Duty: Black Ops II" ]
    // Adaptation to fit our G term.
    const half4 c0 = { -1, -0.0275, -0.572, 0.022 };
    const half4 c1 = { 1, 0.0425, 1.04, -0.04 };
    half4 r = Roughness * c0 + c1;
    half a004 = min( r.x * r.x, exp2( -9.28 * NoV ) ) * r.x + r.y;
    half2 AB = half2( -1.04, 1.04 ) * a004 + r.zw;

    // Anything less than 2% is physically impossible and is instead considered to be shadowing
    // Note: this is needed for the 'specular' show flag to work, since it uses a SpecularColor of 0
    AB.y *= saturate( 50.0 * SpecularColor.g );
    
    return SpecularColor * AB.x + AB.y;
}
#endif