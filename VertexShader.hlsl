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
};

cbuffer ExternalData : register(b0)
{
	// RECALL THAT DATA IN THIS COMES IN MULTIPLES OF 16 !!!!
    float4 colorTint;
    matrix world;	
	matrix view;
	matrix projection;
}

VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;
    
	matrix wvp = mul(projection, mul(view, world));
	output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
	output.normal = input.normal;
    output.uv = input.uv;
	
	return output;
}