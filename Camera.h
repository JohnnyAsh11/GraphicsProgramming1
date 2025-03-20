#ifndef __CAMERA_H_
#define __CAMERA_H_

#include "Transform.h"

class Camera
{
private:
	float m_fFOV;
	Transform m_tTransform;
	DirectX::XMFLOAT4X4 m_m4View;
	DirectX::XMFLOAT4X4 m_m4Projection;

public:
	Camera(float a_fAspectRatio, DirectX::XMFLOAT3 a_v3StartingPosition, float a_fFOV);

	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();
	Transform GetTransform();

	void UpdateProjection(float a_fAspectRatio);
	void UpdateView();

	void Update(float a_fDeltaTime);
};

#endif //__CAMERA_H_
