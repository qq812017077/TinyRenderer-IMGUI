#include "../Basic.hlsli"

float4 main(VS_OUTPUT ps_in) : SV_Target
{
    clip(0.0);
    return float4(0.0, 0.0, 0.0, 0.0);
}