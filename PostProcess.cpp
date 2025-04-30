#include "PostProcess.h"
#include "Graphics.h"
#include "Window.h"

PostProcess::PostProcess(std::shared_ptr<SimplePixelShader> a_pPixelShader)
{
	m_pPixelShader = a_pPixelShader;

	// Describing the texture to be created.
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = Window::Width();
	textureDesc.Height = Window::Height();
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	// Create the resource texture itself.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	Graphics::Device->CreateTexture2D(&textureDesc, 0, pTexture.GetAddressOf());
	
	// Create the Render Target View.
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	Graphics::Device->CreateRenderTargetView(
		pTexture.Get(),
		&rtvDesc,
		m_pResourceTarget.ReleaseAndGetAddressOf());

	// Create the Shader Resource View:
	// By passing it a null description for the SRV, we
	// get a "default" SRV that has access to the entire resource.
	Graphics::Device->CreateShaderResourceView(
		pTexture.Get(),
		0,
		m_pShaderResource.ReleaseAndGetAddressOf());
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> PostProcess::GetRTV(void)
{
	return m_pResourceTarget;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> PostProcess::GetSRV(void)
{
	return m_pShaderResource;
}

std::shared_ptr<SimplePixelShader> PostProcess::GetPixelShader(void)
{
	return m_pPixelShader;
}

void PostProcess::PreRender(float a_fBackgroundColor[4])
{
	// Clearing the extra render target at the start of every frame.
	Graphics::Context->ClearRenderTargetView(m_pResourceTarget.Get(), a_fBackgroundColor);

	// Swapping the active render targets.
	Graphics::Context->OMSetRenderTargets(1, m_pResourceTarget.GetAddressOf(), Graphics::DepthBufferDSV.Get());
}

void PostProcess::PostRender(std::shared_ptr<SimpleVertexShader> a_pVertexShader, Microsoft::WRL::ComPtr<ID3D11SamplerState> a_pSampler)
{
	Graphics::Context->OMSetRenderTargets(1, Graphics::BackBufferRTV.GetAddressOf(), 0);

	// Setting the active shaders.
	a_pVertexShader->SetShader();
	m_pPixelShader->SetShader();
	
	// Setting the data for the pixel shader.
	m_pPixelShader->SetShaderResourceView("Pixels", m_pShaderResource.Get());
	m_pPixelShader->SetSamplerState("ClampSampler", a_pSampler.Get());

	// Drawing exactly one triangle.
	Graphics::Context->Draw(3, 0);
}
