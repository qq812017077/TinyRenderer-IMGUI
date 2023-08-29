#include "Basic.hlsli"

// variables
// float4 color = float4(1.0, 1.0, 1.0, 1.0);
cbuffer MatCBuf : register(b3) // 160 because of float4 alignment
{
    float4 color;
}
float4 main(VS_OUTPUT ps_in) : SV_Target
{
    return float4(color.rgba);
}