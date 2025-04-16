#ifndef __LIGHTING_H_
#define __LIGHTING_H_

#include "ShaderFunctions.hlsli"

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
    float a_fIntensity,
    float3 a_v3CameraPosition,
    float a_fRoughness)
{
    // Calculating the relfection and view vectors.
    float3 R = reflect(a_v3Direction, a_v3Normal);
    float3 V = normalize(a_v3CameraPosition - a_v3WorldPos);
    
    // Determining the passed in spec power.
    float specPower = (1.0f - a_fRoughness) * MAX_SPECULAR_EXPONENT;
    
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
    float3 a_v3WorldPos,
    float3 a_v3CameraPosition,
    float a_fRoughness)
{
    return Diffuse(a_lCurrentLight, a_v3Direction, a_v3Normal, a_v3SurfaceColor) +
           SpecularHighlight(
                a_lCurrentLight.Direction, 
                a_v3Normal,
                a_v3WorldPos,
                a_lCurrentLight.Intensity,
                a_v3CameraPosition,
                a_fRoughness) *
           a_v3SurfaceColor;
}

float3 PointLight(
    Light a_lCurrentLight,
    float3 a_v3WorldPos,
    float3 a_v3Normal,
    float3 a_v3SurfaceColor,
    float3 a_v3CameraPosition,
    float a_fRoughness)
{
    // Calculating the vector to the light from the pixel.
    float3 lightToWorld = normalize(a_lCurrentLight.Position - a_v3WorldPos);
           
    // Calculating the attenuation for the point light.
    float attenuation = Attenuate(a_lCurrentLight, a_v3WorldPos);
            
    // Finally, calculating the diffuse and spec with the attenuation scale.
    float3 diffuseTerm = Diffuse(a_lCurrentLight, -lightToWorld, a_v3Normal, a_v3SurfaceColor) * attenuation;
    float3 specTerm = SpecularHighlight(
                        lightToWorld, 
                        a_v3Normal,
                        a_v3WorldPos,
                        a_lCurrentLight.Intensity,
                        a_v3CameraPosition,
                        a_fRoughness) * attenuation;
    
    return diffuseTerm + specTerm;
}

#endif //__LIGHTING_H_