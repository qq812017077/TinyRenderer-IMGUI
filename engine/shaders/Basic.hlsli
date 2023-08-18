
struct VS_INPUT
{
    float3 pos : Position;
    float3 color: Color;
    float3 normal: NORMAL;
    float2 tex: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_Position;
	float3 color : Color;
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};

cbuffer FrameCBuf : register(b0)
{
	matrix g_View;
	matrix g_Proj;
};

cbuffer MaterialCBuf : register(b1)
{
	float3 diffuse;
};