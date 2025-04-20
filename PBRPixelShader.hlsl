#include "ShaderFunctions.hlsli"
#include "PBRFunctions.hlsli"
#define MAX_LIGHT_COUNT 5

Texture2D Albedo : register(t0); // 't' register is specifically for textures.
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
Texture2D ShadowMap : register(t4);

SamplerState BasicSampler : register(s0); // 's' register is specifically for samplers.
SamplerComparisonState ShadowSampler : register(s1);

cbuffer ExternalData : register(b0)
{
    // - -
    float2 scale;
    float2 offset;
    // - -
    float3 cameraPosition;
    float padding;
    // - -
    Light lights[MAX_LIGHT_COUNT];
}

float4 main(VertexToPixel input) : SV_TARGET
{
    // Perform the perspective divide (divide by W) ourselves
    input.shadowMapPos /= input.shadowMapPos.w;
    
    // Convert the normalized device coordinates to UVs for sampling
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; // Flip the Y
    
    // Grab the distances we need: light-to-pixel and closest-surface
    float distToLight = input.shadowMapPos.z;
    
    // Get a ratio of comparison results using SampleCmpLevelZero()
    float shadowAmount = ShadowMap.SampleCmpLevelZero(
            ShadowSampler,
            shadowUV,
            distToLight).r;
    
    // Making sure that the normals are normalized and that the ambient is black.
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
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv * scale + offset).r;
    
    // Getting the metalness valuse from the metalness map.
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv * scale + offset).r;
    
    // Specular color determination -----------------
    // Assume albedo texture is actually holding specular color where metalness == 1
    // Note the use of lerp here - metal is generally 0 or 1, but might be in between
    // because of linear texture sampling, so we lerp the specular color to match
    float3 specularColor = lerp(F0_NON_METAL, albedoColor.rgb, metalness);
    
    float3 total;
    float3 toCamera = normalize(cameraPosition - input.worldPos);
    Light light = { };
    for (int i = 0; i < MAX_LIGHT_COUNT; i++)
    {
        Light currentLight = lights[i];        
        float3 toLight = normalize(currentLight.Position - input.worldPos);
        
        // Calculating different light amounts.
        float3 fFresnel;    
        float diff = DiffusePBR(input.normal, toLight);
        float3 PBR = MicrofacetBRDF(
                    input.normal,
                    toLight,
                    toCamera,
                    roughness,
                    specularColor,
                    fFresnel);
        
        // For the first light, multiply by the shadow amount.
        if (i == 0 && shadowAmount < 0.91f)
        {
            diff *= shadowAmount;
        }
        
        // Calculate diffuse with energy conservation, including cutting diffuse for metals
        float3 balancedDiff = DiffuseEnergyConserve(diff, fFresnel, metalness);
        
        // Combing the total afflicted lighting.
        total += (balancedDiff * albedoColor + PBR) * currentLight.Intensity * currentLight.Color;
    }
    
	return float4(total, 1.0f);
}