#include "Basic.hlsli"

// variables
// float4 color = float4(1.0, 1.0, 1.0, 1.0);
cbuffer MatCBuf : register(b3) // 160 because of float4 alignment
{
    float4 color;
}
float4 main(VS_OUTPUT ps_in) : SV_Target
{
    // note use shader language
    return lerp(ps_in.color, color, step(0.01, color)); // if color is 0,0,0,0, return ps_in.color, else return color
}