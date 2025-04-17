#ifndef __TEXTURE_H_
#define __TEXTURE_H_

#include <wrl/client.h>
#include <d3d11.h>

/// <summary>
/// Container that holds all of the textures needed for a PBR material.
/// </summary>
struct TextureSet
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Albedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Normal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Metal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Roughness;
};

#endif //__TEXTURE_H_

