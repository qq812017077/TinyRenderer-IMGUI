#include "../Basic.hlsli"

struct SKYBOX_VS_OUTPUT
{
    float3 worldPos : POSITION;
    float4 pos : SV_Position;
};

SKYBOX_VS_OUTPUT main(float3 pos : Position)
{
    SKYBOX_VS_OUTPUT output;
    output.worldPos = pos;
    output.pos = mul(float4(pos, 1.0f), g_ViewProj);    // w is zero, because we don't want to translate the skybox
    // output.pos = mul(float4(pos, 1.0f), g_Proj);
    // output.pos.z = 0.0f; // hack: to force depth is 1.0f
    return output;
}