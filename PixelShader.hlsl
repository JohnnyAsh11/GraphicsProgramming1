#include "ShaderFunctions.hlsli"
#define MAX_LIGHT_COUNT 5

Texture2D SurfaceTexture : register(t0); // 't' register is specifically for textures.
SamplerState BasicSampler : register(s0); // 's' register is specifically for samplers.

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

float3 Diffuse(Light a_lCurrentLight, float3 a_v3Direction, float3 a_v3Normal, float3 a_v3SurfaceColor)
{
    // Calculating the diffuse color.
    float3 diffuseColor = a_lCurrentLight.Color * a_lCurrentLight.Intensity * a_v3SurfaceColor;
    
    // Now calculating the diffuse term.
    return max(dot(a_v3Normal, -a_v3Direction), 0) * diffuseColor;
}

float SpecularHighlight(float3 a_v3Direction, float3 a_v3Normal, float3 a_v3WorldPos)
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
        spec = pow(max(dot(R, V), 0.0f), specPower);
    }
    
    return spec;
}

float Attenuate(Light a_lCurrentLight, float3 a_v3WorldPos)
{
    float dist = distance(a_lCurrentLight.Position, a_v3WorldPos);
    float att = saturate(1.0f - (dist * dist / (a_lCurrentLight.Range * a_lCurrentLight.Range)));
    return att * att;
}

float4 main(VertexToPixel input) : SV_TARGET
{	
    input.normal = normalize(input.normal);
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv * scale + offset);
    
    // Looping through all lights and calculating their effects.
    float3 totalLight = ambient;    
    for (int i = 0; i < MAX_LIGHT_COUNT; i++)
    {
        // Calculating the diffuse and specular.
        float3 diffuseTerm;
        float3 specTerm;
        float attenuation = 1.0f;
        
        if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
        {
            diffuseTerm = Diffuse(lights[i], lights[i].Direction, input.normal, surfaceColor);
            specTerm = SpecularHighlight(lights[i].Direction, input.normal, input.worldPos);
        }
        else if (lights[i].Type == LIGHT_TYPE_POINT)
        {
            // Calculating the vector to the light from the pixel.
            float3 lightToWorld = normalize(lights[i].Position - input.worldPos);
            
            // Calculating the attenuation for the point light.
            attenuation = Attenuate(lights[i], input.worldPos);
            
            // Finally, calculating the diffuse and spec with the attenuation scale.
            diffuseTerm = Diffuse(lights[i], -lightToWorld, input.normal, surfaceColor) * attenuation;
            specTerm = SpecularHighlight(lightToWorld, input.normal, input.worldPos) * attenuation;
        }
        
        // combining all aspects of the light and surface.
        totalLight += diffuseTerm + specTerm * surfaceColor;
    }
    
    return float4(totalLight, 1.0f);
}