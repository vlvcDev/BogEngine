

// StarFieldPS.hlsl
struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

static const int NUM_STARS = 3;
static const float GRID_SIZE = 30.0;

float hash(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * float3(443.897, 441.423, 437.195));
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float4 main(PSInput input) : SV_TARGET
{
    float2 uv = input.texCoord;
    float3 col = float3(0,0,0);

    // Calculate base cell
    float2 cell = floor(uv * GRID_SIZE);

    // Loop through neighboring cells
    [unroll]
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float2 cellOffset = float2(x, y);
            float2 pos = cell + cellOffset;

            // Get random position within cell
            float2 starPos = (pos + float2(hash(pos), hash(pos + 1.0))) / GRID_SIZE;

            // Calculate star
            float2 delta = uv - starPos;
            float dist = length(delta);
            float brightness = hash(pos + 2.0);
            float star = brightness * 0.025 / (dist + 0.01);

            col += star * float3(0.8, 0.9, 1.0);
        }
    }

    return float4(saturate(col), 1.0);
}