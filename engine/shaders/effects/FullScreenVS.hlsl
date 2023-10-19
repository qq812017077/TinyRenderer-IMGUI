// fullscreen 
struct VS_OUTPUT
{
	float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT main(float2 pos : Position)
{
	VS_OUTPUT output;
	output.pos = float4(pos.xy, 0.0f, 1.0f);
    // map to [0,1] range
    output.uv = float2((pos.x + 1.0f) / 2.0f, (1.0f - pos.y) / 2.0f);
	return output;
}