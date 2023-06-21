struct VS_OUTPUT
{
	float3 color : Color;
	float4 pos : SV_Position;
};
cbuffer CBuf
{
	matrix rotate;
	matrix scale;
}
VS_OUTPUT main(float2 pos : Position, float3 color: Color)
{

	VS_OUTPUT output;
	output.pos = mul(float4(pos.x, pos.y, 0.0, 1.0), rotate);
	output.pos = mul(output.pos, scale);
	output.color = color;
	return output;
}