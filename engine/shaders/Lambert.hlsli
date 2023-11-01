#ifndef __LAMBERT_H__
#define __LAMBERT_H__

#include "Math.hlsli"
#include "Basic.hlsli"
#include "Shadow.hlsli"


float3 get_directional_light(VS_OUTPUT ps_in, DirectionalLight dirLight, float3 texColor)
{
  float3 normal = ps_in.normal;
  float3 worldPos = ps_in.worldPos;
  float3 lightdir = normalize(dirLight.dir);
  float3 light_color = dirLight.color.rgb * texColor;
  float3 vToL = -lightdir;
  float3 vReflect = reflect(lightdir, normal);
  float3 vView = normalize(g_EyePos - worldPos);
  
  float3 diffuse = light_color * saturate(dot(normal, vToL));
  float3 specular = light_color * pow(saturate(dot(vReflect, vView)), 10);

  float filterSize = 1.0 / float(SAMPLE_SIZE); //SAMPLE_SIZE should be size of Shadow Map,so it equals to  '1 / textureSize(shadowMap, 0)'
  float visibility = DIR_PCF(ps_in.lightPos / ps_in.lightPos.w, filterSize,  0.005);
  // float visibility = useShadowMap(ps_in.lightPos / ps_in.lightPos.w, 0.005);
  // float visibility = PCSS(ps_in.lightPos / ps_in.lightPos.w, filterSize,  0.005);
  return (diffuse + specular) * visibility;
}

float3 get_point_light(VS_OUTPUT ps_in, PointLight pointLight, float3 texColor)
{
  float3 normal = ps_in.normal;
  float3 worldPos = ps_in.worldPos;
  
  float3 lightdir = worldPos - pointLight.pos;
  float distance = length(lightdir);
  lightdir = normalize(lightdir);
  float x = distance / pointLight.range;
  float attenuation = saturate(1.0f - x * x * x * x);

  if(attenuation == 0.0f) return float3(0.0f, 0.0f, 0.0f);

  float3 light_color = pointLight.color.rgb * texColor * attenuation;
  float3 vToL = -lightdir;
  float3 vReflect = reflect(lightdir, normal);
  float3 vView = normalize(g_EyePos - worldPos);
  float3 diffuse = light_color * saturate(dot(normal, vToL));
  float3 specular = light_color * pow(saturate(dot(vReflect, vView)), 10);
  
  float filterSize = 1.0 / float(SAMPLE_SIZE); //SAMPLE_SIZE should be size of Shadow Map,so it equals to  '1 / textureSize(shadowMap, 0)'
  float visibility = POINT_SHADOW(worldPos - pointLight.pos, pointLight.range,  0.005);
  return (diffuse + specular) * visibility;
}

#endif // !__LAMBERT_H__