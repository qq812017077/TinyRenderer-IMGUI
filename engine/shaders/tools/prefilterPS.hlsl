
TextureCube _MainTex : register(t0);
SamplerState sampler_MainTex : register(s0);
#define PI 3.14159265359

cbuffer PerFrameCBuf : register(b4)
{
    float roughness = 0.0f;
};

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 Hammersley(uint i, uint N)
{
    return float2(float(i)/float(N), RadicalInverse_VdC(i));
}

// ----------------------------------------------------------------------------
float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    float a = roughness*roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // from spherical coordinates to cartesian coordinates
    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    float3 up        = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent   = normalize(cross(up, N));
    float3 bitangent = cross(N, tangent);

    float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float4 main(float3 worldPos : POSITION) : SV_TARGET
{
    float3 N = normalize(worldPos);     // normal
    float3 R = N;                       // reflect
    float3 V = R;                       // view
    
    const uint SAMPLE_COUNT = 1024u;
    float TotalWeight = 0.0;   
    float3 PrefilteredColor = float3(0.0f, 0.0f, 0.0f);
    
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley(i, SAMPLE_COUNT);
        float3 H  = ImportanceSampleGGX(Xi, N, roughness).xyz;
        float3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NoL = saturate(dot(N, L));
        if(NoL > 0.0)
        {   
            PrefilteredColor += _MainTex.Sample(sampler_MainTex, L).rgb * NoL;
            TotalWeight      += NoL;
        }
    }
    PrefilteredColor = PrefilteredColor / TotalWeight;
    return float4(PrefilteredColor, 1.0f);
}