#include "Entity.h"
#include "BufferStructs.h"
#include "Graphics.h"

Entity::Entity(Mesh* a_pMesh)
{
	m_pMesh = std::make_shared<Mesh>(*a_pMesh);
	m_tTransform = Transform();
}

Transform& Entity::GetTransform() { return m_tTransform; }
std::shared_ptr<Mesh> Entity::GetMesh() { return std::shared_ptr<Mesh>(); }

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> a_pConstantBuffer, std::shared_ptr<Camera> a_pCamera)
{
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

	// Rendering the mesh to the window.
	m_pMesh->Draw();

}
