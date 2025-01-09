
cbuffer MatrixBuffer
{
    matrix World;
    matrix View;
    matrix Projection;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut VShader(float4 position : POSITION, float4 normal : NORMAL0, float2 uv : TEXCOORD0)
{
    VOut output;

    float4 worldPosition = mul(position, World);
    float4 viewPosition = mul(worldPosition, View);
    output.position = mul(viewPosition, Projection);
    
    output.color = normal;

    return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}