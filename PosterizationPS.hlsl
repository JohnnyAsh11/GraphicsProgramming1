struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{    
    // Grabbing the current pixel color.
    float4 pixelColor = Pixels.Sample(ClampSampler, input.uv);
    
    // The number of different unique bands there are.
    float levels = 5;
    
    // Mapping the current pixel color to a greyscale value.
    float greyscale = max(pixelColor.r, max(pixelColor.g, pixelColor.b));
    
    // Mapping the greyscale to its lower level.
    float lower = floor(greyscale * levels) / levels;
    float lowerDiff = abs(greyscale - lower);
    
    // Doing the same with the upper bounds.
    float upper = ceil(greyscale * levels) / levels;
    float upperDiff = abs(upper - greyscale);
    
    // Calculating the closest level to the lower and upper greyscale values.
    float level = lowerDiff <= upperDiff ? lower : upper;
    float adjustment = level / greyscale;
    
    // Using the calculated adjustment to alter the current pixel color.
    return pixelColor.rgba * adjustment;
}