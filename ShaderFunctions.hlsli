#ifndef __SHADERFUNCTIONS_H_
#define __SHADERFUNCTIONS_H_

struct VertexShaderInput
{
    float3 localPosition : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float worldPos : POSITION;
};

#endif //__SHADERFUNCTIONS_H_