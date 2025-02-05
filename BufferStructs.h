#include <d3d11.h>

using namespace DirectX;

/// <summary>
/// Buffer vertex containing the Color followed by an Offset.
/// </summary>
struct VertexColorOffsetData
{
	XMFLOAT4 m_v4Color;
	XMFLOAT3 m_v3Offset;
};