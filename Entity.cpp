#include "Entity.h"
#include "Graphics.h"
#include "Window.h"

Entity::Entity(std::shared_ptr<Mesh> a_pMesh, std::shared_ptr<Material> a_pMaterial)
{
	m_pMesh = a_pMesh;
	m_pMaterial = a_pMaterial;
	m_tTransform = Transform();
}

// Getters
Transform& Entity::GetTransform() { return m_tTransform; }
std::shared_ptr<Mesh> Entity::GetMesh() { return m_pMesh; }
std::shared_ptr<Material> Entity::GetMaterial() { return m_pMaterial; }

// Setters
void Entity::SetMaterial(std::shared_ptr<Material> a_pMaterial)
{
	m_pMaterial.reset();
	m_pMaterial = std::make_shared<Material>(*a_pMaterial);
}

void Entity::Draw(std::shared_ptr<Camera> a_pCamera, float a_fTotalTime)
{
	m_pMaterial->GetVertexShader()->SetShader();
	m_pMaterial->GetPixelShader()->SetShader();
	m_pMaterial->PrepMaterialForDraw();

	// Vertex shader data setting.
	std::shared_ptr<SimpleVertexShader> vs = m_pMaterial->GetVertexShader();
	vs->SetMatrix4x4("world", m_tTransform.GetWorldMatrix());	// The world matrix of the entity.
	vs->SetMatrix4x4("worldInvTranspose", m_tTransform.GetWorldInverseTransposeMatrix());	
	vs->SetMatrix4x4("view", a_pCamera->GetView());				// The view matrix of the entity.
	vs->SetMatrix4x4("projection", a_pCamera->GetProjection());	// The projection matrix of the entity.
	vs->CopyAllBufferData();

	// Pixel shader data setting.
	std::shared_ptr<SimplePixelShader>ps = m_pMaterial->GetPixelShader();
	ps->SetFloat4("colorTint", m_pMaterial->GetColor());		// The color tint of the entity.
	ps->SetFloat("totalTime", a_fTotalTime);					// The total time that has elapsed in the program.
	ps->SetFloat3("cameraPosition", a_pCamera.get()->GetTransform().GetPosition());
	ps->SetFloat("roughness", m_pMaterial->GetRoughness());

	// TODO: make the scale and offset both fields of the Material.
	ps->SetFloat2("scale", m_pMaterial->GetScale());		// The scale of the texture in the shader. 
	ps->SetFloat2("offset", m_pMaterial->GetOffset());		// The offset of the texture in the shader.
	ps->CopyAllBufferData();

	// Rendering the mesh to the window.
	m_pMesh->Draw();
}
