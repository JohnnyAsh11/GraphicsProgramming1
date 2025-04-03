#include "ShaderFunctions.hlsli"

cbuffer ExternalData : register(b0)
{
	// - -
    float4 colorTint;
	// - -
    matrix world;	
	// - -
    matrix worldInvTranspose;
	// - -
	matrix view;
	// - -
	matrix projection;
}

VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;
    
	matrix wvp = mul(projection, mul(view, world));
	output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
    output.normal = normalize(mul((float3x3) worldInvTranspose, input.normal));
    output.uv = input.uv;
    output.worldPos = mul(world, float4(input.localPosition, 1.0f)).xyz;
    output.tangent = normalize(mul((float3x3) world, input.tangent));
	
	return output;
}