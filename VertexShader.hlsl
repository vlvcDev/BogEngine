cbuffer cbPerObject : register(b0)
{
    matrix worldViewProj;
    matrix world;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.position = mul(float4(input.position, 1.0f), worldViewProj);
    output.color = (output.position + 1.0) / 2;
    return output;
}
