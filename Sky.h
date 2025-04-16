#ifndef __SKY_H_
#define __SKY_H_

#include "Mesh.h"
#include "SimpleShader.h"
#include "Camera.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

/// <summary>
/// Creates a skybox for the world.
/// </summary>
class Sky
{
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencil;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>  m_pRasterizer;
	std::shared_ptr<Mesh> m_pSkyMesh;
	std::shared_ptr<SimplePixelShader> m_pPixelShader;
	std::shared_ptr<SimpleVertexShader> m_pVertexShader;

public:
	/// <summary>
	/// The default constructor for the Sky box object.
	/// </summary>
	Sky(std::shared_ptr<Mesh> a_pMesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> a_pSampler);

	/// <summary>
	/// Renders the skybox to the world.
	/// </summary>
	void Draw(std::shared_ptr<Camera> a_pCamera);

	// --------------------------------------------------------
	// Loads six individual textures (the six faces of a cube map), then
	// creates a blank cube map and copies each of the six textures to
	// another face.  Afterwards, creates a shader resource view for
	// the cube map and cleans up all of the temporary resources.
	// --------------------------------------------------------
	void CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
};

#endif //__SKY_H_

