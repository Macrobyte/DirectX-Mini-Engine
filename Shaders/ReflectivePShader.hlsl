Texture2D texture0 : register(t0);
sampler sampler0;

TextureCube skybox0 : register(t1);

float4 main(float4 position : SV_Position, float4 color : COLOR, float2 uv : TEXCOORD0, float3 reflectUVW : TEXCOORD1) : SV_TARGET
{
    //return texture0.Sample(sampler0, uv);
    float4 sampled = texture0.Sample(sampler0, uv);
    float4 relfectedSampled = skybox0.Sample(sampler0, reflectUVW);
    float4 combined = ((color * sampled * 0.1) + (relfectedSampled * 0.9));
    return saturate(combined);
}