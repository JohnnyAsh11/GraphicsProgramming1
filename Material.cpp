#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> a_pVertexShader, std::shared_ptr<SimplePixelShader> a_pPixelShader, DirectX::XMFLOAT4 a_v4ColorTint)
{
	m_pVertexShader = a_pVertexShader;
	m_pPixelShader = a_pPixelShader;
	m_v4ColorTint = a_v4ColorTint;
}

Material::~Material()
{
	m_pVertexShader.reset();
	m_pPixelShader.reset();
}

Material::Material(const Material& a_pOther)
{
	// Setting the fields.
	m_pVertexShader = a_pOther.m_pVertexShader;
	m_pPixelShader = a_pOther.m_pPixelShader;
	m_v4ColorTint = a_pOther.m_v4ColorTint;
}

Material& Material::operator=(const Material& a_pOther)
{
	// Reallocating memory where necessary.
	if (m_pVertexShader != nullptr) m_pVertexShader.reset();
	if (m_pPixelShader != nullptr) m_pPixelShader.reset();

	// Setting the fields.
	m_pVertexShader = a_pOther.m_pVertexShader;
	m_pPixelShader = a_pOther.m_pPixelShader;
	m_v4ColorTint = a_pOther.m_v4ColorTint;

	return *this;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader() { return m_pVertexShader; }
std::shared_ptr<SimplePixelShader> Material::GetPixelShader() { return m_pPixelShader; }
DirectX::XMFLOAT4 Material::GetColor() { return m_v4ColorTint; }

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> a_pVertexShader) { m_pVertexShader = a_pVertexShader; }
void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> a_pPixelShader) { m_pPixelShader = a_pPixelShader; }
void Material::SetColor(DirectX::XMFLOAT4 a_v4ColorTint) { m_v4ColorTint = a_v4ColorTint; }
