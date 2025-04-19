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
/// 
/// </summary>
class ShadowManager
{
private:
	std::shared_ptr<SimpleVertexShader> m_pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pShadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShadowSRV;
	DirectX::XMFLOAT4X4 m_m4LightViewMatrix;
	DirectX::XMFLOAT4X4 m_m4LightProjectionMatrix;

public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="m_v3LightDirection"></param>
	ShadowManager(DirectX::XMFLOAT3 a_v3LightDirection);

	/// <summary>
	/// 
	/// </summary>
	void Draw(std::vector<Entity> a_lActiveEntities);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShadowSRV();
};

#endif //__SHADOWMANAGER_H_
