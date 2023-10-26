#include "../Basic.hlsli"

float4 main(SHADOW_VS_OUTPUT ps_in) : SV_Target
{
    return ps_in.pos.z;
}