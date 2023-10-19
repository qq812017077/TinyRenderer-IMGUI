// fullscreen 
struct VS_OUTPUT
{
	float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

Texture2D _MainTex;
SamplerState _MainTexSampler;

static const int r = 3;
static const float divisor = (2 * r + 1) * (2 * r + 1);
float4 main(VS_OUTPUT ps_in) : SV_Target
{
    uint width, height;
    _MainTex.GetDimensions(width, height);
    const float dx = 1.0f / width;
    const float dy = 1.0f / height;
    float4 acc = float4(0, 0, 0, 0);
    for(int i = -r; i <= r; ++i)
    {
        for(int j = -r; j <= r; ++j)
        {
            acc += _MainTex.Sample(_MainTexSampler, ps_in.uv + float2(i * dx, j * dy)).rgba;
        }
    }
    return acc / divisor;
}