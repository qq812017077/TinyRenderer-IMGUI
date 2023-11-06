#ifndef _BASIC_H_
#define _BASIC_H_

struct DirectionalLight //32  because of float4 alignment
{
    float3 dir;
    float4 color;
};

struct PointLight   //48 because of float4 alignment
{
    float3 pos;
    float4 color;
    float atten;
    float range;
};

struct SpotLight    //64 because of float4 alignment
{
    float3 pos;
    float3 dir;
    float4 color;
    float range;
    float angle;
};

struct VS_INPUT
{
    float3 pos : Position;
    float4 color: Color;
    float3 normal: NORMAL;
    float2 tex: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_Position;
	float4 color : Color;
    float3 worldPos : POSITION;
    // float4 lightPos : POSITION1;
    float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};

struct SHADOW_VS_OUTPUT
{
	float4 pos : SV_Position;
    float4 viewPos : POSITION;
};

cbuffer PerFrameCBuf : register(b0)
{
    matrix g_ViewProj;
    matrix g_View;
    matrix g_Proj;
};

cbuffer PerDrawCBuf : register(b1)
{
    matrix g_World;
    matrix g_WorldInv;
	matrix g_LightView;
	matrix g_LightProj;
    matrix g_LightViewProj;
    uint   g_goid;
};

cbuffer LightingCBuf : register(b2) // 160 because of float4 alignment
{
    DirectionalLight g_DirLight;    // 32
    matrix g_DirLightViewProjs[4];     // 64
    float4 g_farPlane;              // 16

    PointLight g_PointLight;        // 48
    SpotLight g_SpotLight;          // 64
    float3 g_EyePos;                // 16
};

Texture2DArray _ShadowMap : register(t0);
// Texture2D _ShadowMap : register(t0);
SamplerComparisonState sampler_ShadowMap : register(s0);

TextureCube _ShadowCubeMap : register(t3);

TextureCube _IrradianceMap : register(t4);
SamplerState sampler_IrradianceMap : register(s4);

TextureCube _PrefilteredEnvMap : register(t5);
SamplerState sampler_PrefilteredEnvMap : register(s5);

Texture2D _BRDFLutMap : register(t6);
SamplerState sampler_BRDFLutMap : register(s6);

#define PCF_RANGE 2

#endif