#ifndef __GAMEENTITY_H_
#define __GAMEENTITY_H_

#include <memory>
#include "Mesh.h"
#include "Camera.h"

class Entity 
{
private:
	std::shared_ptr<Mesh> m_pMesh;
	Transform m_tTransform;

public:
	Entity(Mesh* a_pMesh);

	Transform& GetTransform();
	std::shared_ptr<Mesh> GetMesh();

	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> a_pConstantBuffer, std::shared_ptr<Camera> a_pCamera);
};

#endif //__GAMEENTITY_H_