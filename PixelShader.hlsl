#include "GeneralLightingFunctions.hlsli"
#define MAX_LIGHT_COUNT 5

Texture2D SurfaceTexture : register(t0);    // 't' register is specifically for textures.
Texture2D NormalMap : register(t1);
SamplerState BasicSampler : register(s0);   // 's' register is specifically for samplers.

cbuffer ExternalData : register(b0)
{
    // - -
    float4 colorTint;
    // - -
    float2 scale;
    float2 offset;
    // - -
    float3 cameraPosition;
    float roughness;
    // - -
    float3 ambient;
    float padding;
    // - -
    Light lights[MAX_LIGHT_COUNT];
}

float4 main(VertexToPixel input) : SV_TARGET
{	
    input.normal = normalize(input.normal);
    float3 surfaceColor = pow(SurfaceTexture.Sample(BasicSampler, input.uv * scale + offset).xyz, 2.2f);
    
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv * scale + offset).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal);
    
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    T = normalize(T - N * dot(T, N));
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    input.normal = mul(unpackedNormal, TBN);
    
    // Looping through all lights and calculating their effects.
    float3 totalLight = ambient * surfaceColor;
    for (int i = 0; i < MAX_LIGHT_COUNT; i++)
    {
        if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
        {
            totalLight += DirectionalLight(
                            lights[i], 
                            lights[i].Direction, 
                            input.normal, 
                            surfaceColor, 
                            input.worldPos,
                            cameraPosition,
                            roughness);
        }
        else if (lights[i].Type == LIGHT_TYPE_POINT)
        {
            totalLight += PointLight(
                            lights[i], 
                            input.worldPos, 
                            input.normal, 
                            surfaceColor,
                            cameraPosition,
                            roughness);
        }
        else if (lights[i].Type == LIGHT_TYPE_SPOT)
        {
            totalLight += PointLight(
                            lights[i],
                            input.worldPos,
                            input.normal,
                            surfaceColor,
                            cameraPosition,
                            roughness) *
                            SpotlightAttenuation(lights[i], input.worldPos);
        }
    }
    
    // Multiply by 2 for the gamma correction.
    return float4(pow(totalLight, 1.0f / 2.2f), 1.0f);
}