struct VIn
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};


struct VOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};


cbuffer CBuffer0
{
    matrix WVP; // 64 bytes
};

VOut main( VIn input )
{
    VOut output;
    output.position = mul(WVP, float4(input.position, 1));
    output.color = input.color;
    output.uv = input.uv;
    return output;
}