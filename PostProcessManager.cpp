#include "PostProcessManager.h"
#include "Graphics.h"
#include "Window.h"
#include "PathHelpers.h"

PostProcessManager::PostProcessManager()
{
	// Creating the sampler state for post processing.
	D3D11_SAMPLER_DESC ppSampDesc = {};
	ppSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ppSampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&ppSampDesc, m_pSampler.GetAddressOf());

	// Loading in the vertex shader for post processing.
	m_pVertexShader = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PostProcessVS.cso").c_str());
}

PostProcessManager::~PostProcessManager()
{
	for (int i = 0; i < m_lPostProcesses.size(); i++)
	{
		delete m_lPostProcesses[i];
	}
}

void PostProcessManager::AddPostProcess(PostProcess* a_pPostProcess)
{
	m_lPostProcesses.push_back(a_pPostProcess);
}

void PostProcessManager::ClearPostProcesses()
{
	m_lPostProcesses.clear();
}

void PostProcessManager::PreRender(float a_fBackgroundColor[4])
{
	for (int i = 0; i < m_lPostProcesses.size(); i++)
	{
		m_lPostProcesses[i]->PreRender(a_fBackgroundColor);
	}
}

void PostProcessManager::PostRender()
{
	for (int i = 0; i < m_lPostProcesses.size(); i++)
	{
		m_lPostProcesses[i]->PostRender(m_pVertexShader, m_pSampler);
	}
}

void PostProcessManager::OnResize()
{
	std::vector<PostProcess*> lNewProcesses = std::vector<PostProcess*>();

	for (int i = 0; i < m_lPostProcesses.size(); i++)
	{
		// Reinstantiating the PostProcesses with the same PixelShader.
		// Should inheritly resize properly.
		std::shared_ptr<SimplePixelShader> pPS = m_lPostProcesses[i]->GetPixelShader();
		lNewProcesses.push_back(new PostProcess(pPS));
	}

	// Clearing memory.
	for (int i = 0; i < m_lPostProcesses.size(); i++)
	{
		delete m_lPostProcesses[i];
		m_lPostProcesses[i] = nullptr;
	}

	// Setting the new list of post procresses.
	m_lPostProcesses = lNewProcesses;
}
