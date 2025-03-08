struct VertexToPixel
{
	float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float randX;
    float randY;
    float resolution;
}

float2 random2(float2 p)
{
    return sin(float2(dot(p, float2(127.1f, 311.7f)), dot(p, float2(269.5f, 183.3f)))) * 43758.5453f;
}

float4 main(VertexToPixel input) : SV_TARGET
{	
    float2 st = input.screenPosition.xy / resolution;
    st.x *= resolution;
    st *= 3.0f;
    
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float m_dist = 1;
    
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            // Neighbor place in the grid
            float2 neighbor = float2(float(x), float(y));

            float2 currentPoint = random2(st + neighbor);

			// Animate the point
            currentPoint = 0.5 + 0.5 * sin(randX + 6.2831 * currentPoint);

			// Vector between the pixel and the point
            float2 diff = neighbor + currentPoint - st;

            // Distance to the point
            float dist = length(diff);

            // Keep the closer distance
            m_dist = min(m_dist, dist);
        }
    }
    
    color += m_dist;
    color += 1.0f - step(0.02f, m_dist);
    color.r += step(0.98f, st.x) + step(0.98f, st.y);
    color -= step(0.7f, abs(sin(27.0f * m_dist))) * 0.5f;
    
    return color;
}

/*

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

void main() {
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;
    vec3 color = vec3(.0);

    // Scale
    st *= 3.;

    // Tile the space
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float m_dist = 1.;  // minimum distance

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            // Neighbor place in the grid
            vec2 neighbor = vec2(float(x),float(y));

            // Random position from current + neighbor place in the grid
            vec2 point = random2(i_st + neighbor);

			// Animate the point
            point = 0.5 + 0.5*sin(u_time + 6.2831*point);

			// Vector between the pixel and the point
            vec2 diff = neighbor + point - f_st;

            // Distance to the point
            float dist = length(diff);

            // Keep the closer distance
            m_dist = min(m_dist, dist);
        }
    }

    // Draw the min distance (distance field)
    color += m_dist;

    // Draw cell center
    color += 1.-step(.02, m_dist);

    // Draw grid
    color.r += step(.98, f_st.x) + step(.98, f_st.y);

    // Show isolines
    // color -= step(.7,abs(sin(27.0*m_dist)))*.5;

    gl_FragColor = vec4(color,1.0);
}


*/