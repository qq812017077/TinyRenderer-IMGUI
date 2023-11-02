#include "../Math.hlsli"
// fullscreen 
struct VS_OUTPUT
{
	float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

Texture2D _MainTex;
SamplerState _MainTexSampler;

float4 main(VS_OUTPUT ps_in) : SV_Target
{
    float4 color = _MainTex.Sample(_MainTexSampler, ps_in.uv);

    return float4(gammaCorrect(color.rgb, 2.2), 1.0f);
    // return color;
}