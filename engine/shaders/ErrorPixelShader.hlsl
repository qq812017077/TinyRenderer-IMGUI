#include "Basic.hlsli"

float4 main(VS_OUTPUT ps_in) : SV_Target
{
    // output pink
    return float4(1.0f, 0.0f, 1.0f, 1.0f);
}