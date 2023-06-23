cbuffer FrameCBuf : register(b0)
{
	matrix g_View;
	matrix g_Proj;
};

cbuffer MaterialCBuf : register(b1)
{
	float3 diffuse;
};

float4 main(float3 color :Color) : SV_Target
{
    return float4(color, 1.0);
}