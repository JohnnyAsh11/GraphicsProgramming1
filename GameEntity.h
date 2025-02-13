#ifndef __GAMEENTITY_H_
#define __GAMEENTITY_H_

#include <memory>
#include "Mesh.h"
#include "Transform.h"

class GameEntity 
{
private:
	std::shared_ptr<Mesh> m_pMesh;
	Transform m_tTransform;

public:
	GameEntity(Mesh* a_pMesh);

	Transform& GetTransform();
	std::shared_ptr<Mesh> GetMesh();

	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> a_pConstantBuffer);
};

#endif //__GAMEENTITY_H_