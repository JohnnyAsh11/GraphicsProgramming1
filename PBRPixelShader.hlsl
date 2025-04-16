#include "ShaderFunctions.hlsli"
#include "PBRFunctions.hlsli"
#define MAX_LIGHT_COUNT 5

Texture2D Albedo : register(t0); // 't' register is specifically for textures.
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
SamplerState BasicSampler : register(s0); // 's' register is specifically for samplers.

cbuffer ExternalData : register(b0)
{
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
    // Making sure that the normals are normalized.
    input.normal = normalize(input.normal);
    
    // Getting the surface/albedo color from the Albedo texture.
    float3 albedoColor = pow(Albedo.Sample(BasicSampler, input.uv * scale + offset).xyz, 2.2f);
    
    // unpacking the normal map and setting its value.
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv * scale + offset).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal);
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    T = normalize(T - N * dot(T, N));
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    input.normal = mul(unpackedNormal, TBN);
    
    // Getting the roughness value from the roughness map.
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    
    // Getting the metalness valuse from the metalness map.
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    
    // Specular color determination -----------------
    // Assume albedo texture is actually holding specular color where metalness == 1
    // Note the use of lerp here - metal is generally 0 or 1, but might be in between
    // because of linear texture sampling, so we lerp the specular color to match
    float3 specularColor = lerp(F0_NON_METAL, albedoColor.rgb, metalness);
    
    for (int i = 0; i < MAX_LIGHT_COUNT; i++)
    {
        Light currentLight = lights[i];
        float fFresnel = 0.0f;
    
        float3 PBR = MicrofacetBRDF(
                    input.normal,
                    normalize(currentLight.Position - input.worldPos),
                    normalize(cameraPosition - input.worldPos),
                    roughness,
                    specularColor);

    }
    
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}