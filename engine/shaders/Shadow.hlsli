#ifndef __SHADOW_H__
#define __SHADOW_H__

#include "Math.hlsli"
#include "Basic.hlsli"
struct DirLightShadowConfig
{
  float   index;
  float4  shadowCoord;
  float   filterSize;
  float   bias;
};

// Soft-Shadow 1
float PCF_Filter(in float2 poissonDisk[PCF_NUM_SAMPLES], DirLightShadowConfig dir_shadow_config)
{
  float current_depth = dir_shadow_config.shadowCoord.z;
  float visibility_sum = 0.0;
  
  float3 shadowCoord = float3(dir_shadow_config.shadowCoord.xy, dir_shadow_config.index);

  [unroll]
  for( int x = -PCF_RANGE; x <= PCF_RANGE; x++)
  {
    [unroll]
    for( int y = -PCF_RANGE; y <= PCF_RANGE; y++)
    {
      visibility_sum += _ShadowMap.SampleCmpLevelZero(sampler_ShadowMap, shadowCoord , current_depth - dir_shadow_config.bias, int2(x,y)).r;
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
    return _ShadowMap.SampleCmpLevelZero(sampler_ShadowMap, float3(shadowCoord.xy, 0.0), shadowCoord.z - bias);
  
}

float DIR_PCF(DirLightShadowConfig dir_shadow_config) 
{
  if(dir_shadow_config.shadowCoord.z > 1.0f || dir_shadow_config.shadowCoord.z < 0.0f) return 1.0;
  float2 poissonDisk[PCF_NUM_SAMPLES];
  poissonDiskSamples(dir_shadow_config.shadowCoord.xy, poissonDisk);
  //uniformDiskSamples(shadowCoord.xy, poissonDisk);
  return PCF_Filter(poissonDisk, dir_shadow_config);
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



float POINT_SHADOW(in float3 dir, float range, float bias)
{
  float current_depth = length(dir) / range;
  return _ShadowCubeMap.SampleCmpLevelZero(sampler_ShadowMap, dir, current_depth).r;
}


#endif