#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Mesh.h"
#include <DirectXMath.h>

using namespace DirectX;

class Game
{
private:
	float m_fBackgroundColor[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	bool m_bDemoVisibility;
	XMFLOAT4 m_fColor1 = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);  // Red.
	XMFLOAT4 m_fColor2 = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);  // Green.
	XMFLOAT4 m_fColor3 = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);  // Blue.

public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void UpdateImGui(float deltaTime);

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Creating 3 Mesh fields:
	Mesh* m_mMesh1;
	Mesh* m_mMesh2;
	Mesh* m_mMesh3;

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

