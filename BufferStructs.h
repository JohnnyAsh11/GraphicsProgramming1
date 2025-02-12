#include <d3d11.h>

using namespace DirectX;

/// <summary>
/// Buffer vertex containing the Color followed by an Offset.
/// </summary>
struct VertexColorWorldData
{
	XMFLOAT4 m_v4Color;
	XMFLOAT4X4 m_m4WorldMatrix;
};