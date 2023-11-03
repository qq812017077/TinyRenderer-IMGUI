TextureCube _MainTex : register(t0);
SamplerState sampler_MainTex : register(s0);
#define PI 3.14159265359

float4 main(float3 worldPos : POSITION) : SV_TARGET
{
    float3 irradiance = float3(0.0, 0.0, 0.0);
    // convolution code
    float3 normal = normalize(worldPos);
    float3 up       = float3(0.0, 1.0, 0.0);
    float3 right    = cross(up, normal);
    up              = cross(normal, right);
    float sampleDelta = 0.025;
    float samplerCount = 0.0; 
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
            irradiance += _MainTex.Sample(sampler_MainTex, sampleVec);
            samplerCount++;
        }
    }
    irradiance = PI * irradiance * (1.0f / samplerCount);
    return float4(irradiance, 1.0);
    // return _MainTex.Sample(sampler_MainTex, worldPos);
}