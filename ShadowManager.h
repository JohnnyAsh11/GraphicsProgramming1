#ifndef __SHADOWMANAGER_H_
#define __SHADOWMANAGER_H_

#include <d3d11.h>
#include <memory>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <vector>

#include "Entity.h"
#include "SimpleShader.h"

/// <summary>
/// Manages the shadow map and light matrices for a single light in the scene.
/// </summary>
class ShadowManager
{
private:
	std::shared_ptr<SimpleVertexShader> m_pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pShadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShadowSRV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pShadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pShadowSampler;
	DirectX::XMFLOAT4X4 m_m4LightViewMatrix;
	DirectX::XMFLOAT4X4 m_m4LightProjectionMatrix;

public:
	/// <summary>
	/// Constructs a ShadowManager with a passed in light direction.
	/// </summary>
	/// <param name="m_v3LightDirection">The direction that the light travels.</param>
	ShadowManager(DirectX::XMFLOAT3 a_v3LightDirection);

	/// <summary>
	/// Creates the shadow map with the active entities in the scene.
	/// </summary>
	void Draw(std::vector<Entity>& a_lActiveEntities);

	/// <summary>
	/// Gets the currently generated shadow map.
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShadowSRV(void);

	/// <summary>
	/// Gets the sampler for rendering shadows.
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetShadowSampler(void);

	/// <summary>
	/// Gets the light's view matrix.
	/// </summary>
	DirectX::XMFLOAT4X4 GetLightView(void);

	/// <summary>
	/// Gets the light's projection matrix.
	/// </summary>
	DirectX::XMFLOAT4X4 GetLightProjection(void);
};

#endif //__SHADOWMANAGER_H_
