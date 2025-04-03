#include "Material.h"

Material::Material(
	std::shared_ptr<SimpleVertexShader> a_pVertexShader,
	std::shared_ptr<SimplePixelShader> a_pPixelShader, 
	DirectX::XMFLOAT4 a_v4ColorTint,
	float a_fRoughness)
{
	m_pVertexShader = a_pVertexShader;
	m_pPixelShader = a_pPixelShader;
	m_v4ColorTint = a_v4ColorTint;
	m_mTextureSRVs = std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
	m_mSamplers = std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>>();
	m_fRoughness = a_fRoughness;
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
	m_mTextureSRVs = a_pOther.m_mTextureSRVs;
	m_mSamplers = a_pOther.m_mSamplers;
	m_fRoughness = a_pOther.m_fRoughness;
	m_fScale = a_pOther.m_fScale;
	m_fOffset = a_pOther.m_fOffset;
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
	m_mTextureSRVs = a_pOther.m_mTextureSRVs;
	m_mSamplers = a_pOther.m_mSamplers;
	m_fScale = a_pOther.m_fScale;
	m_fOffset = a_pOther.m_fOffset;

	return *this;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader() { return m_pVertexShader; }
std::shared_ptr<SimplePixelShader> Material::GetPixelShader() { return m_pPixelShader; }
DirectX::XMFLOAT4 Material::GetColor() { return m_v4ColorTint; }
float Material::GetRoughness() { return m_fRoughness; }

DirectX::XMFLOAT2 Material::GetScale()
{
	return m_fScale;
}

DirectX::XMFLOAT2 Material::GetOffset()
{
	return m_fOffset;
}

void Material::SetScale(DirectX::XMFLOAT2 a_fScale)
{
	m_fScale = a_fScale;
}

void Material::SetOffset(DirectX::XMFLOAT2 a_fOffset)
{
	m_fOffset = a_fOffset;
}

std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Material::GetTextures()
{ return m_mTextureSRVs; }

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> a_pVertexShader) { m_pVertexShader = a_pVertexShader; }
void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> a_pPixelShader) { m_pPixelShader = a_pPixelShader; }
void Material::SetColor(DirectX::XMFLOAT4 a_v4ColorTint) { m_v4ColorTint = a_v4ColorTint; }

void Material::AddTexturesSRV(std::string a_sTextureName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> a_pSRV)
{
	m_mTextureSRVs.insert({ a_sTextureName, a_pSRV });
}

void Material::AddSampler(std::string a_sSamplerName, Microsoft::WRL::ComPtr<ID3D11SamplerState> a_pSampler)
{
	m_mSamplers.insert({ a_sSamplerName, a_pSampler });
}

void Material::PrepMaterialForDraw()
{
	for (const auto& t : m_mTextureSRVs) { m_pPixelShader->SetShaderResourceView(t.first.c_str(), t.second); }
	for (const auto& s : m_mSamplers) { m_pPixelShader->SetSamplerState(s.first.c_str(), s.second); }
}
