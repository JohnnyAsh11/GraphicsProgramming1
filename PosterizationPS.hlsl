struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{    
    float4 pixelColor = Pixels.Sample(ClampSampler, input.uv);
    float levels = 10;
    float greyscale = max(pixelColor.r, max(pixelColor.g, pixelColor.b));
    float lower = floor(greyscale * levels) / levels;
    float lowerDiff = abs(greyscale - lower);
    float upper = ceil(greyscale * levels) / levels;
    float upperDiff = abs(upper - greyscale);
    float level = lowerDiff <= upperDiff ? lower : upper;
    float adjustment = level / greyscale;
    
    return pixelColor.rgba * adjustment;
}