cbuffer cbPerObject : register(b0)
{
    float4x4 worldViewProj;
    float4x4 world;
    float4 lightDirection;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 color : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // Normalize the normal and light direction vectors
    float3 normalizedNormal = normalize(input.normal);
    float3 normalizedLightDir = normalize(float3(0.4, 1.0, -0.7));

    // Calculate the diffuse lighting component
    float diffuse = max(0.0, dot(normalizedNormal, normalizedLightDir));

    // Generate a pseudo-random value based on the normal vector
    float dither = frac(sin(dot(normalizedNormal, float3(12.9898,78.233,45.164))) * 43758.5453);

    // Adjust the diffuse level based on dithering
    float levels = 4.0;
    float quantizedDiffuse = floor((diffuse + dither / levels) * levels) / (levels - 1.0);

    // Calculate the final color with quantized diffuse lighting
    float3 finalColor = quantizedDiffuse * input.color;

    // Edge detection based on normal variation
    float edgeThreshold = 0.2;
    float edge = length(fwidth(normalizedNormal));
    if (edge > edgeThreshold)
    {
        finalColor = float3(0.9, 0.0, 0.0); // Black outline
    }

    return float4(finalColor, 1.0);
}
