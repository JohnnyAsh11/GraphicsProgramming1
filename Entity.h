#ifndef __GAMEENTITY_H_
#define __GAMEENTITY_H_

#include <memory>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

class Entity 
{
private:
	std::shared_ptr<Mesh> m_pMesh;
	std::shared_ptr<Material> m_pMaterial;
	Transform m_tTransform;

public:
	Entity(Mesh* a_pMesh, Material* a_pMaterial);

	Transform& GetTransform();
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial();

	void SetMaterial(Material* a_pMaterial);

	void Draw(std::shared_ptr<Camera> a_pCamera, float a_fTotalTime);
};

#endif //__GAMEENTITY_H_