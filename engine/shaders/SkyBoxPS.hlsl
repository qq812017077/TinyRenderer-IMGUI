TextureCube _MainTex : register(t0);
SamplerState sampler_MainTex : register(s0);


float4 main(float3 worldPos : POSITION) : SV_TARGET
{
    return _MainTex.Sample(sampler_MainTex, worldPos);
}