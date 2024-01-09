#define MAX_POINT_LIGHTS 8

struct PointLight
{
    float4 colour;
    float4 position;
    float strength;
    bool enabled;
    float2 padding;
};

struct VIn
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
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
	float4 ambientLightCol;
	float4 directionalLightDir;
    float4 directionalLightCol;
    //PointLight pointLights[MAX_POINT_LIGHTS];
    PointLight pointLight;
    
    float2 tiling;  // 16 bytes
    float2 padding; // 16 bytes
};

VOut main( VIn input )
{
    VOut output;

    // Position
    output.position = mul(WVP, float4(input.position, 1));
    // Texture Coords
    output.uv = input.uv * tiling;

    // Lighting
    float diffuseAmount = dot(directionalLightDir.xyz, input.normal);
    diffuseAmount = saturate(diffuseAmount);

    float3 directionalFinal = directionalLightCol * diffuseAmount;

    // Point Light
    float3 pointFinal = float3(0, 0, 0);
    
    if(pointLight.enabled == true)
    {
        float3 pointLightDir = normalize(pointLight.position - float4(input.position, 1));
        float pointLightDistance = length(pointLight.position - float4(input.position, 1));
        float pointLightAttenuation = pointLight.strength / (pointLightDistance * pointLightDistance + pointLight.strength); // A = Strength / d^2+Strength
        float pointAmount = dot(pointLightDir.xyz, input.normal) * pointLightAttenuation;
        pointAmount = saturate(pointAmount);
        pointFinal += pointLight.colour * pointAmount;
    }
    
    
    
    //for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    //{
	   // if(!pointLights[i].enabled)
    //        continue;

    //    float3 pointLightDir = normalize(pointLights[i].position - float4(input.position, 0));
    //    float pointLightDistance = length(pointLights[i].position - float4(input.position, 0));
    //    float pointLightAttenuation = pointLights[i].strength / (pointLightDistance * pointLightDistance + pointLights[i].strength); // A = Strength / d^2+Strength
    //    float pointAmount = dot(pointLightDir.xyz, input.normal) * pointLightAttenuation;
    //    pointAmount = saturate(pointAmount);
    //    pointFinal += pointLights[i].colour * pointAmount * 6;

    //}
	

    // Final Colour
    output.color = saturate(ambientLightCol + float4(directionalFinal, 1) + float4(pointFinal, 1));

    return output;
}