#include "Entity.h"
#include "Graphics.h"

Entity::Entity(Mesh* a_pMesh, Material* a_pMaterial)
{
	m_pMesh = std::make_shared<Mesh>(*a_pMesh);
	m_pMaterial = std::make_shared<Material>(*a_pMaterial);
	m_tTransform = Transform();
}

// Getters
Transform& Entity::GetTransform() { return m_tTransform; }
std::shared_ptr<Mesh> Entity::GetMesh() { return std::shared_ptr<Mesh>(); }
std::shared_ptr<Material> Entity::GetMaterial() { return m_pMaterial; }

// Setters
void Entity::SetMaterial(std::shared_ptr<Material> a_pMaterial) { m_pMaterial = a_pMaterial; }

void Entity::Draw(std::shared_ptr<Camera> a_pCamera)
{
	m_pMaterial->GetVertexShader()->SetShader();
	m_pMaterial->GetPixelShader()->SetShader();

#pragma region constant buffer code
/*
	// Sending data to GPU with the constant buffer.
	// 1. Collect data (Creating a data transfer object)
	VertexColorWorldData DTO{};
	DTO.m_v4Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DTO.m_m4WorldMatrix = m_tTransform.GetWorldMatrix();
	DTO.m_m4ViewMatrix = a_pCamera->GetView();
	DTO.m_m4ProjectionMatrix = a_pCamera->GetProjection();

	// 2. Copy to GPU with memcpy
	// Creating a mapped subresource struct to hold the cbuffer GPU address
	D3D11_MAPPED_SUBRESOURCE mapped{};

	// Actually grabbing the cbuffer's address
	Graphics::Context->Map(
		a_pConstantBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);

	// Copying the data to the GPU
	memcpy(mapped.pData, &DTO, sizeof(VertexColorWorldData));

	// Unmapping from the memory address.
	Graphics::Context->Unmap(a_pConstantBuffer.Get(), 0);
	*/
#pragma endregion
	std::shared_ptr<SimpleVertexShader> vs = m_pMaterial->GetVertexShader();
	vs->SetFloat4("colorTint", m_pMaterial->GetColor());		// The color tint of the entity.
	vs->SetMatrix4x4("world", m_tTransform.GetWorldMatrix());	// The world matrix of the entity.
	vs->SetMatrix4x4("view", a_pCamera->GetView());				// The view matrix of the entity.
	vs->SetMatrix4x4("projection", a_pCamera->GetProjection());	// The projection matrix of the entity.
	vs->CopyAllBufferData();

	// Rendering the mesh to the window.
	m_pMesh->Draw();

}
