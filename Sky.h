#ifndef __SKY_H_
#define __SKY_H_

#include "Mesh.h"
#include "SimpleShader.h"

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

};

#endif //__SKY_H_

