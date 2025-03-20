Texture2D SurfaceTexture : register(t0); // 't' register is specifically for textures.
SamplerState BasicSampler : register(s0); // 's' register is specifically for samplers.

struct VertexToPixel
{
	float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float2 scale;
    float2 offset;
}

float4 main(VertexToPixel input) : SV_TARGET
{	
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv * scale + offset);
    return surfaceColor * colorTint;
}