#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <vector>

#include "Entity.h"
#include "Camera.h"
#include "SimpleShader.h"

class Game
{
private:
	float m_fBackgroundColor[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	bool m_bDemoVisibility;

	std::vector<std::shared_ptr<Camera>> m_lCameras;
	std::shared_ptr<Camera> m_pActiveCamera = nullptr;

	std::vector<Entity> m_lEntities;

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
	void UpdateImGui(float deltaTime);

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Creating 3 Mesh fields:
	Mesh* m_mMesh1 = nullptr;
	Mesh* m_mMesh2 = nullptr;
	Mesh* m_mMesh3 = nullptr;
};

