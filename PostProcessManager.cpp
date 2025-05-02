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
	for (std::pair p : m_lPostProcesses)
	{
		delete p.second;
	}
}

void PostProcessManager::AddPostProcess(PostProcessType a_dUniqueEnum, PostProcess* a_pPostProcess)
{
	m_lPostProcesses.insert({ a_dUniqueEnum, a_pPostProcess });
}

void PostProcessManager::ClearPostProcesses()
{
	m_lPostProcesses.clear();
}

void PostProcessManager::PreRender(float a_fBackgroundColor[4])
{
	// Skip if there are no active post processes.
	if (m_dActivePP == PostProcessType::None) return;

	for (std::pair p : m_lPostProcesses)
	{
		// Skipping all but the active post process effect.
		if (m_dActivePP != p.first) continue;

		p.second->PreRender(a_fBackgroundColor);
	}
}

void PostProcessManager::PostRender()
{
	// Skip if there are no active post processes.
	if (m_dActivePP == PostProcessType::None) return;

	for (std::pair p : m_lPostProcesses)
	{
		// Skipping all but the active post process effect.
		if (m_dActivePP != p.first) continue;

		p.second->PostRender(m_pVertexShader, m_pSampler);
	}
}

void PostProcessManager::SetActiveProcess(PostProcessType a_dPostProcess) { m_dActivePP = a_dPostProcess; }

void PostProcessManager::OnResize()
{
	std::unordered_map<PostProcessType, PostProcess*> lNewProcesses = std::unordered_map<PostProcessType, PostProcess*>();

	for (std::pair p : m_lPostProcesses)
	{
		// Reinstantiating the PostProcesses with the same PixelShader.
		// Should inheritly resize properly.
		std::shared_ptr<SimplePixelShader> pPS = p.second->GetPixelShader();
		lNewProcesses.insert({ p.first, new PostProcess(pPS) });
	}

	// Clearing memory.
	for (std::pair p : m_lPostProcesses)
	{
		delete p.second;
		p.second = nullptr;
	}

	// Setting the new list of post procresses.
	m_lPostProcesses = lNewProcesses;
}
