#include "Basic.hlsli"

float4 main(VS_OUTPUT ps_in) : SV_Target
{
    return float4(ps_in.color, 1.0);
}