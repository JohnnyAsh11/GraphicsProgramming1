
// Changing code options based on the active compiler:
#ifndef HLSL
#define VECTOR4 float4
#else
#define VECTOR4 DirectX::XMFLOAT4
#endif

// RenderDoc -> Shader debugging for everything.