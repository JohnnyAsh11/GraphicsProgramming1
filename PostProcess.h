#ifndef __POSTPROCESS_H_
#define __POSTPROCESS_H_

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

#include "SimpleShader.h"

/// <summary>
/// Contains the Pixel Shader and Render targets for a single post process effect.
/// </summary>
class PostProcess
{
private:
	std::shared_ptr<SimplePixelShader> m_pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pResourceTarget;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResource;

public:
	/// <summary>
	/// Initializes a post process object.
	/// </summary>
	/// <param name="a_pPixelShader">The pixel shader used by this post process effect.</param>
	PostProcess(std::shared_ptr<SimplePixelShader> a_pPixelShader);

	/// <summary>
	/// Gets the render target view.
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRTV(void);

	/// <summary>
	/// Gets the shader resource view.
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV(void);

	/// <summary>
	/// Gets the pixel shader used by this post process.
	/// </summary>
	std::shared_ptr<SimplePixelShader> GetPixelShader(void);

	/// <summary>
	/// Clears all necessary buffers from the pipeline.
	/// </summary>
	/// <param name="a_fBackgroundColor">The background color of the application.</param>
	void PreRender(float a_fBackgroundColor[4]);

	/// <summary>
	/// Renders the post process effect.
	/// </summary>
	/// <param name="a_pVertexShader">The post process vertex shader.</param>
	/// <param name="a_pSampler">The post process sampler object.</param>
	void PostRender(std::shared_ptr<SimpleVertexShader> a_pVertexShader, Microsoft::WRL::ComPtr<ID3D11SamplerState> a_pSampler);
};

#endif //__POSTPROCESS_H_
