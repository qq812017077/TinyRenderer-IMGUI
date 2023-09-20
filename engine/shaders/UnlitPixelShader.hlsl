#include "Basic.hlsli"

float4 color = float4(1.0, 1.0, 1.0, 1.0);

float4 main(VS_OUTPUT ps_in) : SV_Target
{
    return color;
}