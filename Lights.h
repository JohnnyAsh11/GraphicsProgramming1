#ifndef __LIGHTS_H_
#define __LIGHTS_H_

#include <DirectXMath.h>

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

/// <summary>
/// Defines all necessary values for a light in the world.
/// </summary>
struct Light 
{
	int Type;							// The type of light.
	DirectX::XMFLOAT3 Direction;		// The direction that the light is pointing.
	float Range;						// Point/Spot lights have a range for attenuation.
	DirectX::XMFLOAT3 Position;			// Point/Spot lights have a position in world space.		
	float Intensity;					// All lights require an intensity.
	DirectX::XMFLOAT3 Color;			// The color of the light.
	float SpotInnerAngle;				// The inner cone in radians.  (light at its brightest)
	float SpotOuterAngle;				// The outher cone in radians. (light fading out)
	DirectX::XMFLOAT2 Padding;			// Purposeful padding for the 16-byte boundaries.
};

#endif //__LIGHTS_H_