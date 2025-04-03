#include "ShaderFunctions.hlsli"
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

float3 Diffuse(
    Light a_lCurrentLight, 
    float3 a_v3Direction, 
    float3 a_v3Normal, 
    float3 a_v3SurfaceColor)
{
    // Calculating the diffuse color.
    float3 diffuseColor = a_lCurrentLight.Color * a_lCurrentLight.Intensity * a_v3SurfaceColor;
    
    // Now calculating the diffuse term.
    return max(dot(a_v3Normal, -a_v3Direction), 0) * diffuseColor;
}

float3 SpecularHighlight(
    float3 a_v3Direction,
    float3 a_v3Normal, 
    float3 a_v3WorldPos,
    float a_fIntensity)
{
    // Calculating the relfection and view vectors.
    float3 R = reflect(a_v3Direction, a_v3Normal);
    float3 V = normalize(cameraPosition - a_v3WorldPos);
    
    // Determining the passed in spec power.
    float specPower = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    
    // Only performing the specular calculations if spec power is notable.
    float spec = 0.0f;
    if (specPower > 0.05f)
    {
        spec = pow(max(dot(R, V), 0.0f), specPower) * a_fIntensity;
    }
    
    return spec;
}

float Attenuate(
    Light a_lCurrentLight, 
    float3 a_v3WorldPos)
{
    float dist = distance(a_lCurrentLight.Position, a_v3WorldPos);
    float att = saturate(1.0f - (dist * dist / (a_lCurrentLight.Range * a_lCurrentLight.Range)));
    return att * att;
}

float SpotlightAttenuation(
    Light a_lCurrentLight,
    float3 a_v3WorldPos)
{
    // Getting a vector from the light to the pixel.
    float3 lightToPixel = normalize(a_v3WorldPos - a_lCurrentLight.Position);    
    float spotCos = dot(lightToPixel, -a_lCurrentLight.Direction);
    
    // Getting the cos of the 2 angles.  They are already expected to be in radians.
    float innerCos = cos(a_lCurrentLight.SpotInnerAngle);
    float outerCos = cos(a_lCurrentLight.SpotOuterAngle);
    
    // Getting the actual attenuation of the spotlight.
    float spotFactor = saturate((spotCos - outerCos) / (innerCos - outerCos));
    return pow(spotFactor, 2);
}

float3 DirectionalLight(
    Light a_lCurrentLight, 
    float3 a_v3Direction, 
    float3 a_v3Normal,
    float3 a_v3SurfaceColor,
    float3 a_v3WorldPos)
{
    return Diffuse(a_lCurrentLight, a_v3Direction, a_v3Normal, a_v3SurfaceColor) + 
           SpecularHighlight(a_lCurrentLight.Direction, a_v3Normal, a_v3WorldPos, a_lCurrentLight.Intensity) *
           a_v3SurfaceColor;
}

float3 PointLight(
    Light a_lCurrentLight,
    float3 a_v3WorldPos,
    float3 a_v3Normal,
    float3 a_v3SurfaceColor)
{
    // Calculating the vector to the light from the pixel.
    float3 lightToWorld = normalize(a_lCurrentLight.Position - a_v3WorldPos);
           
    // Calculating the attenuation for the point light.
    float attenuation = Attenuate(a_lCurrentLight, a_v3WorldPos);
            
    // Finally, calculating the diffuse and spec with the attenuation scale.
    float3 diffuseTerm = Diffuse(a_lCurrentLight, -lightToWorld, a_v3Normal, a_v3SurfaceColor) * attenuation;
    float3 specTerm = SpecularHighlight(lightToWorld, a_v3Normal, a_v3WorldPos, a_lCurrentLight.Intensity) * attenuation;
    
    return diffuseTerm + specTerm;
}

float4 main(VertexToPixel input) : SV_TARGET
{	
    input.normal = normalize(input.normal);
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv * scale + offset).xyz;
    
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
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
                            input.worldPos);
        }
        else if (lights[i].Type == LIGHT_TYPE_POINT)
        {
            totalLight += PointLight(
                            lights[i], 
                            input.worldPos, 
                            input.normal, 
                            surfaceColor);
        }
        else if (lights[i].Type == LIGHT_TYPE_SPOT)
        {
            totalLight += PointLight(
                            lights[i],
                            input.worldPos,
                            input.normal,
                            surfaceColor) * 
                            SpotlightAttenuation(lights[i], input.worldPos);
        }
    }
    
    return float4(totalLight, 1.0f);
}