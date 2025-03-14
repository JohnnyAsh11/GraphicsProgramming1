#ifndef __MATERIAL_H_
#define __MATERIAL_H_

#include <memory>
#include "SimpleShader.h"

class Material
{
private:
	std::shared_ptr<SimpleVertexShader> m_pVertexShader;
	std::shared_ptr<SimplePixelShader> m_pPixelShader;
	DirectX::XMFLOAT4 m_v4ColorTint;

public:
	/// <summary>
	/// Constructs an instance of a Material for applying shaders to specific Meshes.
	/// </summary>
	/// <param name="a_pVertexShader">The vertex shader for objects in the world.</param>
	/// <param name="a_pPixelShader">The pixel shader for objects in the world.</param>
	/// <param name="a_v4ColorTint">The color tint for objects in the world.</param>
	Material(
		std::shared_ptr<SimpleVertexShader> a_pVertexShader, 
		std::shared_ptr<SimplePixelShader> a_pPixelShader,
		DirectX::XMFLOAT4 a_v4ColorTint);

	/// <summary>
	/// Destructor for the Material class.
	/// </summary>
	~Material();

	/// <summary>
	/// Copy constructor for the Material class.
	/// </summary>
	Material(const Material& a_pOther);

	/// <summary>
	/// Copy operator for the Material class.
	/// </summary>
	Material& operator=(const Material& a_pOther);

	/// <summary>
	/// Gets the currently set vertex shader.
	/// </summary>
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	/// <summary>
	/// Gets the currently set pixel shader.
	/// </summary>
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	/// <summary>
	/// Gets the color tint of the material.
	/// </summary>
	DirectX::XMFLOAT4 GetColor();

	/// <summary>
	/// Sets the vertex shader.
	/// </summary>
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> a_pVertexShader);
	/// <summary>
	/// Sets the pixel shader.
	/// </summary>
	void SetPixelShader(std::shared_ptr<SimplePixelShader> a_pPixelShader);
	/// <summary>
	/// Sets the color tint.
	/// </summary>
	void SetColor(DirectX::XMFLOAT4 a_v4ColorTint);
};

#endif //__MATERIAL_H_

