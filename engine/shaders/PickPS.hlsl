#include "Basic.hlsli"

uint4 main(VS_OUTPUT ps_in) : SV_Target
{
    return uint4(g_goid, 0, 0, 0);
}