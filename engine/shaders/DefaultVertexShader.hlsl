struct VS_OUTPUT
{
	float3 color : Color;
	float4 pos : SV_Position;
};

cbuffer FrameCBuf : register(b0)
{
	matrix g_View;
	matrix g_Proj;
};
cbuffer ObjectCBuf : register(b2)
{
	matrix g_World;
};

VS_OUTPUT main(float3 pos : Position, float3 color: Color)
{

	VS_OUTPUT output;
	output.pos = mul(float4(pos.x, pos.y, pos.z, 1.0), g_World);
	output.pos = mul(output.pos, g_View);
	output.pos = mul(output.pos, g_Proj);
	output.color = color;
	return output;
}