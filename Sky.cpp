#include "Sky.h"

Sky::Sky(std::shared_ptr<Mesh> a_pMesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> a_pSampler)
{
	m_pSkyMesh = std::make_shared<Mesh>(*a_pMesh);
}
	