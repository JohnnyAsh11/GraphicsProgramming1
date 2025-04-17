
// Changing code options based on the active compiler:
#ifndef HLSL
#define VECTOR4 float4
#else
#define VECTOR4 DirectX::XMFLOAT4
#endif

// RenderDoc -> Shader debugging for everything.


/*
post processing pixel shader motion blur psuedocode.

pixelWidth = 1.0f / windowWidth
pixelHeight = 1.0f / windowHeight

int sampleCount
float3 totalColor = 0.0f;

for (int -blur; int < blur; int++)
	for (int -blur; int < blur; int++)
		float2 uv = input.uv + float2(pixelWidth * x, pixelHeight * y);
		totalColor += Sample(uv);
		sampleCount++;

return float4(totalColor / sampleCount, 0.0f);

*/