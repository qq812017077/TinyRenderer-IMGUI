#include "Basic.hlsli"
#include "Math.hlsli"
float4 color = float4(1.0, 1.0, 1.0, 1.0);

Texture2D _MainTex : register(t1);
SamplerState sampler_MainTex : register(s1);

// Soft-Shadow 1
float PCF_Filter(in float2 poissonDisk[PCF_NUM_SAMPLES], float4 shadowCoord, float filterSize, float bias)
{
  float current_depth = shadowCoord.z;
  float visibility_sum = 0.0;
  
  // for( int i = 0; i < PCF_NUM_SAMPLES; i++)
  // {
  //   float2 offset = poissonDisk[i] * filterSize;  //filterSize 
  //   // float depth = _ShadowMap.Sample(sampler_ShadowMap, shadowCoord.xy + offset).r;
  //   // visibility_sum += (((depth + bias) > current_depth) ? 1.0 : 0.0);
  // }
  // return visibility_sum / float(PCF_NUM_SAMPLES);
  [unroll]
  for( int x = -PCF_RANGE; x <= PCF_RANGE; x++)
  {
    [unroll]
    for( int y = -PCF_RANGE; y <= PCF_RANGE; y++)
    {
      visibility_sum += _ShadowMap.SampleCmpLevelZero(sampler_ShadowMap, shadowCoord.xy, current_depth - bias, int2(x,y)).r;
    }
  }
  int sample_num = (PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1);
  return visibility_sum / float(sample_num);
}

//get avg depth from light to block
// float findBlocker(in float2 poissonDisk[PCF_NUM_SAMPLES],  float2 uv,  float zReceiver, float bias) {
//   poissonDiskSamples(uv, poissonDisk);
//   float sum_block_depth = 0.0;
//   int blockNum = 0;
//   float searchWidth =  10.0 / float(SAMPLE_SIZE);
//   //float searchWidth = LIGHT_SIZE_UV  * max( 0.0, (zReceiver - NEAR_PLANE) / zReceiver) * 0.5;
//   for( int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; i++)
//   {
//     float2 offset = poissonDisk[i] * searchWidth;  //filterSize 
//     float depth = _ShadowMap.Sample(sampler_ShadowMap, uv + offset).r;
//     if((depth + bias) < zReceiver)
//     {
//       sum_block_depth += depth;
//       blockNum++;
//     }
//   }

//   if(blockNum == 0) return -0.1;
//   if(blockNum == BLOCKER_SEARCH_NUM_SAMPLES) return 1.5;
//   return sum_block_depth / float(blockNum);
// }

// Hard-Shadow
float useShadowMap(float4 shadowCoord, float bias){
  
  if(shadowCoord.z > 1.0f) return 1.0;
  // float depth = _ShadowMap.Sample(sampler_ShadowMap, shadowCoord.xy).r;
  // return (depth + bias) > shadowCoord.z ? 1.0 : 0.0;

  return _ShadowMap.SampleCmpLevelZero(sampler_ShadowMap, shadowCoord.xy, shadowCoord.z - bias);
  
}

float PCF(in float4 shadowCoord, float filterSize, float bias) 
{
  if(shadowCoord.z > 1.0f || shadowCoord.z < 0.0f) return 1.0;
  float2 poissonDisk[PCF_NUM_SAMPLES];
  poissonDiskSamples(shadowCoord.xy, poissonDisk);
  //uniformDiskSamples(shadowCoord.xy, poissonDisk);
  return PCF_Filter(poissonDisk, shadowCoord, filterSize * 5, bias);
}

// Soft-Shadow 2
// float PCSS(float4 shadowCoord, float filterSize, float bias){

//   float2 poissonDisk[PCF_NUM_SAMPLES];
//   // STEP 1: avgblocker depth
//   float zReceiver = shadowCoord.z;
//   float avg_depth_block = findBlocker(poissonDisk, shadowCoord.xy, zReceiver, bias);
//   if(avg_depth_block <EPS) return 1.0;
//   if(avg_depth_block > 1.0) return 0.0;
//   //return 0.5;
//   // STEP 2: penumbra size
//   float penumbra = LIGHT_SIZE_UV * (zReceiver - avg_depth_block) / avg_depth_block;
//   filterSize = penumbra * 0.3;
//   // STEP 3: filtering
//   return PCF_Filter(poissonDisk, shadowCoord, filterSize * 5, bias);
// }

float3 get_directional_light(float3 normal, float3 worldPos, DirectionalLight dirLight, float3 texColor)
{
    float3 lightdir = normalize(dirLight.dir);
    float3 light_color = dirLight.color.rgb * texColor;
    float3 vToL = -lightdir;
    float3 vReflect = reflect(lightdir, normal);
    float3 vView = normalize(g_EyePos - worldPos);
    
    float3 diffuse = light_color * saturate(dot(normal, vToL));
    float3 specular = light_color * pow(saturate(dot(vReflect, vView)), 10);
    return diffuse + specular;
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
    float3 diffuse = light_color * saturate(dot(normal, vToL));
    float3 specular = light_color * pow(saturate(dot(vReflect, vView)), 10);
    return diffuse + specular;
}


float4 main(VS_OUTPUT ps_in) : SV_Target
{
    // phong shading
    float3 vNormal = normalize(ps_in.normal);
	float4 texColor = _MainTex.Sample(sampler_MainTex, ps_in.tex);
    texColor.rgb = float3(texColor.r * color.r, texColor.g * color.g, texColor.b * color.b);

    float3 ambient = texColor.rgb * float3(0.06, 0.06, 0.06);
    // directional light
    float3 dir_color = get_directional_light(vNormal, ps_in.worldPos, g_DirLight, texColor.rgb);
    // point light
    float3 point_color = get_point_light(vNormal, ps_in.worldPos, g_PointLight, texColor.rgb);

    float filterSize = 1.0 / float(SAMPLE_SIZE); //SAMPLE_SIZE should be size of Shadow Map,so it equals to  '1 / textureSize(shadowMap, 0)'
    // float visibility = useShadowMap(ps_in.lightPos / ps_in.lightPos.w, 0.005);
    float visibility = PCF(ps_in.lightPos / ps_in.lightPos.w, filterSize,  0.005);
    // float visibility = PCSS(ps_in.lightPos / ps_in.lightPos.w, filterSize,  0.005);
    float3 color = dir_color + point_color;
    return float4(ambient + color * visibility, 1.0);
}