#include "ShadowManager.h"

#include "Window.h"
#include "Graphics.h"
#include "PathHelpers.h"

#define SHADOW_MAP_RESOLUTION 1024

ShadowManager::ShadowManager(DirectX::XMFLOAT3 a_v3LightDirection)
{
	// Making sure that the direction is normalized.
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&a_v3LightDirection);
	vec = DirectX::XMVector3Normalize(vec);
	DirectX::XMStoreFloat3(&a_v3LightDirection, vec);

	m_m4LightViewMatrix = DirectX::XMFLOAT4X4();
	m_m4LightProjectionMatrix = DirectX::XMFLOAT4X4();	
	
	// Creating the shadow texture.
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = SHADOW_MAP_RESOLUTION;
	shadowDesc.Height = SHADOW_MAP_RESOLUTION;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	Graphics::Device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

	// Binding the shadow map to the pipeline through the depth stencil and SRV.
	// Create the depth/stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	Graphics::Device->CreateDepthStencilView(
		shadowTexture.Get(),
		&shadowDSDesc,
		m_pShadowDSV.GetAddressOf());

	// Create the SRV for the shadow map.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Graphics::Device->CreateShaderResourceView(
		shadowTexture.Get(),
		&srvDesc,
		m_pShadowSRV.GetAddressOf());

	// Creating the position vector.
	// TODO: This view matrix changes every time the light's position changes.
	float dBackup = -20;
	DirectX::XMFLOAT3 v3 = DirectX::XMFLOAT3(
								a_v3LightDirection.x * dBackup, 
								a_v3LightDirection.y * dBackup, 
								a_v3LightDirection.z * dBackup);
	DirectX::XMMATRIX lightView = DirectX::XMMatrixLookToLH(
		DirectX::XMLoadFloat3(&v3),						// Position: "Backing up" 20 units from origin
		DirectX::XMLoadFloat3(&a_v3LightDirection),		// Direction: light's direction
		DirectX::XMVectorSet(0, 1, 0, 0));				// Up: World up vector (Y axis)

	float fProjectionSize = 15.0f;
	DirectX::XMMATRIX lightProjection = DirectX::XMMatrixOrthographicLH(
		fProjectionSize,
		fProjectionSize,
		1.0f,
		100.0f);

	// Storing the calculated matrix values.
	DirectX::XMStoreFloat4x4(&m_m4LightViewMatrix, lightView);
	DirectX::XMStoreFloat4x4(&m_m4LightProjectionMatrix, lightProjection);

	// Loading in the vertex shader.
	m_pVertexShader = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"ShadowVertex.cso").c_str());
}

void ShadowManager::Draw(std::vector<Entity> a_lActiveEntities)
{
	// Clearing the shadow map.
	Graphics::Context->ClearDepthStencilView(m_pShadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	// Setting up the output merger stage.
	ID3D11RenderTargetView* nullRTV{};
	Graphics::Context->OMSetRenderTargets(1, &nullRTV, m_pShadowDSV.Get());

	// Halting pixel processing completely.
	Graphics::Context->PSSetShader(0, 0, 0);

	// Setting the viewport for the rasterizer.
	D3D11_VIEWPORT viewport = {};
	viewport.Width = (float)SHADOW_MAP_RESOLUTION;
	viewport.Height = (float)SHADOW_MAP_RESOLUTION;
	viewport.MaxDepth = 1.0f;
	Graphics::Context->RSSetViewports(1, &viewport);

	// Setting some of the shader data.
	m_pVertexShader->SetShader();
	m_pVertexShader->SetMatrix4x4("view", m_m4LightViewMatrix);
	m_pVertexShader->SetMatrix4x4("projection", m_m4LightProjectionMatrix);

	// Loop and draw all entities.
	for (auto& e : a_lActiveEntities)
	{
		// Setting the world matrix and copying the buffer data over.
		m_pVertexShader->SetMatrix4x4("world", e.GetTransform().GetWorldMatrix());
		m_pVertexShader->CopyAllBufferData();

		// Draw the mesh directly to avoid the entity's material.
		e.GetMesh()->Draw();
	}

	// Reseting the pipeline for actual rendering.
	viewport.Width = (float)Window::Width();
	viewport.Height = (float)Window::Height();
	Graphics::Context->RSSetViewports(1, &viewport);
	Graphics::Context->OMSetRenderTargets(
		1,
		Graphics::BackBufferRTV.GetAddressOf(),
		Graphics::DepthBufferDSV.Get());
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShadowManager::GetShadowSRV() { return m_pShadowSRV; }
