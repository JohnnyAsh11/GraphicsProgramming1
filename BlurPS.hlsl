struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    // Track the total color and number of samples
    float4 total = 0;
    int sampleCount = 0;
    
    // Loop through the "box"
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            // Calculate the uv for this sample
            float2 uv = input.uv;
            uv += float2(x * 0.0025f, y * 0.0025f);
            
            // Add this color to the running total
            total += Pixels.Sample(ClampSampler, uv);
            sampleCount++;
        }
    }
    
    // Return the average
    return total / sampleCount;
}