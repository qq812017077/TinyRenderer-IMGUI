Texture2D _MainTex : register(t0);
SamplerState sampler_MainTex : register(s0);

float2 SampleSphericalMap(float3 v)
{
    const float2 InvAtan = float2(0.1591, 0.3183);
    float2 UV = float2(atan2(v.z, v.x), asin(v.y));
    UV *= InvAtan;
    UV += 0.5;
    return UV;
}
float4 main(float3 worldPos : POSITION) : SV_TARGET
{
    float2 uv = SampleSphericalMap(normalize(worldPos)); // make sure to normalize localPos
    float3 color = _MainTex.Sample(sampler_MainTex, uv).rgb;
    return float4(color, 1);
}