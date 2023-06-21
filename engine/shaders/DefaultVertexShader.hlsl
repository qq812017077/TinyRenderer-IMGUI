struct VS_OUTPUT
{
	float3 color : Color;
	float4 pos : SV_Position;
};

cbuffer ObjectCBuf : register(b2)
{
	matrix world;
};

VS_OUTPUT main(float3 pos : Position, float3 color: Color)
{

	VS_OUTPUT output;
	// output.pos = mul(float4(pos.x, pos.y, pos.z, 1.0), world);
	output.pos = float4(pos.x, pos.y, pos.z, 1.0);
	output.color = color;
	return output;
}