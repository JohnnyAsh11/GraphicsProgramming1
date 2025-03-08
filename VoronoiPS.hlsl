struct VertexToPixel
{
	float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float totalTime;
}

// RandomVector helper function that I found online: https://iquilezles.org/articles/voronoilines/
// My understanding is that it is using the UV coords as the equivalent of a random number.
float2 randomVector(float2 UV, float offset)
{
    float2x2 m = float2x2(15.27f, 47.63f, 99.41f, 89.98f);
    UV = frac(sin(mul(UV, m)) * 46839.32f);
    return float2(sin(UV.y * (+offset)) * 0.5f + 0.5f, cos(UV.x * offset) * 0.5f + 0.5f);
}

float4 main(VertexToPixel input) : SV_TARGET
{
    // Control variables for the algorithm.
    float angleOffset = totalTime * 10.0f;
    float cellDensity = 5.0f;
    
    // Variables for Voronoi.
    float distFromCenter = 8.0f;
    float distFromEdge = 8.0f;  
    int2 cell = floor(input.uv * cellDensity);
    float2 posInCell = frac(input.uv * cellDensity);    
    float2 closestOffset;
    
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            // Creating the current cell to check.
            int2 cellToCheck = int2(x, y);
            
            // Generating a random vector and adding that onto the difference of the position in the Cell and cell itself. 
            float2 cellOffset = float2(cellToCheck) - posInCell + randomVector(cell + cellToCheck, angleOffset);
            
            // Not 100% how this line works, it was apart of the psuedocode I found though.
            float distToPoint = dot(cellOffset, cellOffset);

            // Keeping only the shortest distance and associated offset to that point.
            if (distToPoint < distFromCenter)
            {
                distFromCenter = distToPoint;
                closestOffset = cellOffset;
            }
        }
    }
    
    // Saving the square root of the distance from the center.
    distFromCenter = sqrt(distFromCenter);
    
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            // Repeating the same beginning process as the previous set of loops.
            int2 cellToCheck = int2(x, y);
            float2 cellOffset = float2(cellToCheck) - posInCell + randomVector(cell + cellToCheck, angleOffset);
            
            // Calculating the distance to the edge of the cell.
            float distToEdge = dot(0.5f * (closestOffset + cellOffset), 
                               normalize(cellOffset - closestOffset));

            // Saving the smallest of those values.
            distFromEdge = min(distFromEdge, distToEdge);
        }
    }
    
    // Storing the 2 calculated distances into any channel of the RGBa.
    return float4(distFromEdge, 0.0f, distFromCenter, 1.0f) * colorTint;
}