struct VIn
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float3 uvw : TEXCOORD;
};

cbuffer CBuffer0
{
    matrix WVP;
};

VOut main(VIn input)
{
    VOut output;
    
    // Position
    output.position = mul(WVP, input.position);
    
    // Texture coords
    output.uvw = input.position.xyz;
    
    output.color = input.color;
    
    return output;
}