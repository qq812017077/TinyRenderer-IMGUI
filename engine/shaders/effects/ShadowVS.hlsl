struct VS_OUTPUT
{
	float4 pos : SV_Position;
};

cbuffer ShadowCBuf : register(b2) // 160 because of float4 alignment
{
    matrix g_LightMVP;
}

VS_OUTPUT main(float3 pos : Position)
{
	VS_OUTPUT output;
	// float3 pos = v_in.pos;
	output.pos = mul(float4(pos, 1.0), g_LightMVP);
	return output;
}