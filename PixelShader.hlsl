Texture2D SurfaceTexture : register(t0); // 't' register is specifically for textures.
SamplerState BasicSampler : register(s0); // 's' register is specifically for samplers.

struct VertexToPixel
{
	float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float worldPos : POSITION;
};

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float2 scale;
    float2 offset;
    float3 cameraPosition;
}

// Lambertian Reflectance
// max(normalized surface, ) * intensity * 

float4 main(VertexToPixel input) : SV_TARGET
{	
    input.normal = normalize(input.normal);
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv * scale + offset);
    
    float3 totalLight = float3(0, 0, 0);
    
    float3 ambientColor = float3(0.25f, 0.25f, 0.25f);
    float3 ambientTerm = ambientColor * surfaceColor;
    
    // Light definition.
    float3 lightColor = float3(1.0f, 1.0f, 1.0f);
    float lightIntensity = 3.0f;
    float3 lightDirection = float3(1, 0, 0);
    
    // Diffuse
    float3 diffuseTerm = 
        max(dot(input.normal, lightDirection), 0) *
        lightColor * lightIntensity * surfaceColor;
    
    // Specular
    float3 refl = reflect(lightDirection, input.normal);
    float3 viewVector = normalize(cameraPosition - input.worldPos);
    
    float3 specTerm = pow(max(dot(refl, viewVector), 0), 64) *
        lightColor * lightIntensity * surfaceColor;
    
    totalLight += ambientTerm + diffuseTerm + specTerm;
    
    return float4(totalLight, 1.0f);
    //return float4(surfaceColor, 1.0f) * colorTint;
}