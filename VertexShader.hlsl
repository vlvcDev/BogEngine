//Vertex Shader
cbuffer cbPerObject : register(b0)
{
    float4x4 worldViewProj;  // 64 bytes
    float4x4 world;          // 64 bytes
    float4 lightDirection;    // 16 bytes
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), worldViewProj);

    // Correct normal transformation
    output.normal = mul(input.normal, (float3x3)world);

    output.color = input.color;
    return output;
}