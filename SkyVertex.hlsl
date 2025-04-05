#include "ShaderFunctions.hlsli"

struct VertexToPixel_Sky
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix projection;
}

VertexToPixel_Sky main(VertexShaderInput input)
{
    VertexToPixel_Sky output;
    
    matrix viewNoTranslation = view;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;    
    matrix viewProj = mul(projection, viewNoTranslation);
    
    output.position = mul(viewProj, float4(input.localPosition, 1.0f));
    output.position.z = output.position.w;
    output.sampleDir = input.localPosition;
    
    return output;
}