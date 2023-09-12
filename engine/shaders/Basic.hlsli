
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
    float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};

cbuffer PerFrameCBuf : register(b0)
{
	matrix g_View;
	matrix g_Proj;
};

cbuffer PerDrawCBuf : register(b1)
{
    matrix g_World;
    matrix g_WorldInv;
};

cbuffer LightingCBuf : register(b2) // 160 because of float4 alignment
{
    DirectionalLight g_DirLight;    // 32
    PointLight g_PointLight;        // 48
    SpotLight g_SpotLight;          // 64
    float3 g_EyePos;                // 16
};                                  // 32 + 48 + 64 + 16 = 160
