#ifndef __SHADERFUNCTIONS_H_
#define __SHADERFUNCTIONS_H_

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_SPECULAR_EXPONENT 256.0f

struct VertexShaderInput
{
    float3 localPosition : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldPos : POSITION;
    float3 tangent : TANGENT;
    float4 shadowMapPos : SHADOW_POSITION;
};

struct Light
{
    // - -
    int Type; 
    float3 Direction;     
    // - -
    float Range;                   
    float3 Position;    
    // - - 
    float Intensity;               
    float3 Color;       
    // - -
    float SpotInnerAngle;          
    float SpotOuterAngle;          
    float2 Padding;     
    // - -
};

#endif //__SHADERFUNCTIONS_H_