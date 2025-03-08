#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"

#include "Colors.h"
#include <vector>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include "Material.h"

// For the DirectX Math library
using namespace DirectX;

void Game::Initialize()
{
	// Seeding the random object.
	srand(time(0));

	// Initializing the Camera.
	m_lCameras.push_back(std::shared_ptr<Camera>(new Camera(Window::AspectRatio(), XMFLOAT3(0.0f, 0.0f, -5.0f), 60.0f)));
	m_lCameras.push_back(std::shared_ptr<Camera>(new Camera(Window::AspectRatio(), XMFLOAT3(2.5f, 0.0f, -5.0f), 45.0f)));
	m_lCameras.push_back(std::shared_ptr<Camera>(new Camera(Window::AspectRatio(), XMFLOAT3(-2.5f, 0.0f, -5.0f), 75.0f)));
	m_pActiveCamera = m_lCameras[0];

	std::shared_ptr<SimpleVertexShader> pBasicVS = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());

	std::shared_ptr<SimplePixelShader> pBasicPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> pUVsPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugUVsPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> pNormalsPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugNormalsPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> pVoronoiPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VoronoiPS.cso").c_str());

	// Creating the materials.
	Material* mat1 = new Material(pBasicVS, pBasicPS, XMFLOAT4(.6f, 0.0f, 1.0f, 1.0f));
	Material* mat2 = new Material(pBasicVS, pUVsPS, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	Material* mat3 = new Material(pBasicVS, pNormalsPS, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	Material* mat4 = new Material(pBasicVS, pVoronoiPS, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

	// Creating the 3D models.
	Mesh* cube = new Mesh("Models/cube.graphics_obj");
	Mesh* cylinder = new Mesh("Models/cylinder.graphics_obj");
	Mesh* sphere = new Mesh("Models/sphere.graphics_obj");
	Mesh* helix = new Mesh("Models/helix.graphics_obj");
	Mesh* torus = new Mesh("Models/torus.graphics_obj");
	Mesh* quad = new Mesh("Models/quad.graphics_obj");
	Mesh* quadDoubleSided = new Mesh("Models/quad_double_sided.graphics_obj");

	// Controls the amount of sets of Entities are created.
	int dAmountOfSets = 4;

	// Instantiating the Entities.
	for (UINT i = 0; i < dAmountOfSets; i++)
	{
		m_lEntities.push_back(Entity(cube, mat1));
		m_lEntities.push_back(Entity(cylinder, mat1));
		m_lEntities.push_back(Entity(sphere, mat1));
		m_lEntities.push_back(Entity(helix, mat1));
		m_lEntities.push_back(Entity(torus, mat1));
		m_lEntities.push_back(Entity(quadDoubleSided, mat1));
		m_lEntities.push_back(Entity(quad, mat1));
	}

	// Deleting all of the meshes.
	delete cube, cylinder, helix, sphere, torus, quad, quadDoubleSided;

	for (UINT j = 0; j < dAmountOfSets; j++)
	{
		for (UINT i = 0; i < m_lEntities.size() / dAmountOfSets; i++)
		{
			int index = i + (j * (m_lEntities.size() / dAmountOfSets));

			// Setting the scale and getting the current Transform.
			float fUniformScale = 0.25f;
			Transform& current = m_lEntities[index].GetTransform();

			// Setting the scale and spacing out the models.
			current.SetScale(fUniformScale, fUniformScale, fUniformScale);
			current.SetPosition((i * 1.25f) - 4.0f, -1.0f + j, 0.0f);
			current.Rotate(XMFLOAT3(0.0f, 2.0f, 0.0f));

			// Setting the proper materials.
			if (j == 0)
			{
				m_lEntities[index].SetMaterial(mat2);
			}
			else if (j == 1)
			{
				m_lEntities[index].SetMaterial(mat3);
			}
			else if (j == 2)
			{
				m_lEntities[index].SetMaterial(mat4);
			}
			else
			{
				m_lEntities[index].SetMaterial(mat1);
			}
		}
	}

	// Deleting all of the materials.
	delete mat1, mat2, mat3, mat4;
	
	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();

	// Setting the Graphics Context to be using Triangle Lists.
	Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Game::OnResize()
{
	// So long as the camera list has content,
	if (m_lCameras.size() != 0)
	{
		// Update the projection matrices.
		for (unsigned int i = 0; i < m_lCameras.size(); i++)
		{
			m_lCameras[i]->UpdateProjection(Window::AspectRatio());
		}
	}
}

void Game::Update(float deltaTime, float totalTime)
{
	UpdateImGui(deltaTime);
	m_pActiveCamera->Update(deltaTime);

	// Performing some fun transformations on one of the entities.
	//Transform& current = m_lEntities[0].GetTransform();
	//current.SetPosition(static_cast<float>(sin(totalTime)), 0.0f, 0.0f);

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE)) Window::Quit();
}

/// <summary>
/// Helper method for updating the logic within ImGui.
/// </summary>
/// <param name="deltaTime"></param>
void Game::UpdateImGui(float deltaTime)
{
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);

	// Setting what is inside of the Gui.
	ImGui::Begin("Application Settings");

	// Framerate:
	ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);
	// Window resolution:
	ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());

	// Editing the color of the background.
	ImGui::ColorEdit4("Background Color", m_fBackgroundColor);

	// Changing the visibility of the ImGui demo window.
	if (m_bDemoVisibility)
	{
		ImGui::ShowDemoWindow();
	}
	if (ImGui::Button("Show ImGui Demo Window"))
	{
		m_bDemoVisibility = !m_bDemoVisibility;
	}

	// Creating a sub section for the entities.
	if (ImGui::TreeNode("Entities"))
	{
		// Looping through the entities in the list.s
		for (unsigned int i = 0; i < m_lEntities.size(); i++)
		{
			// Interface naming.
			std::string sInterface = "Entity " + std::to_string(i);
			sInterface += "##";
			sInterface += std::to_string(i);

			// Creating the nodes for the individual entities.
			if (ImGui::TreeNode(sInterface.c_str()))
			{
				// Getting the current entity's transform.
				Transform& current = m_lEntities[i].GetTransform();

				// Creating the drag floats.
				ImGui::DragFloat3(
					("Position##" + std::to_string(i)).c_str(), 
					&current.GetPosition().x,
					0.05f);
				ImGui::DragFloat3(
					("Rotation##" + std::to_string(i)).c_str(),
					&current.GetRotation().x,
					0.05f);
				ImGui::DragFloat3(
					("Scale##" + std::to_string(i)).c_str(),
					&current.GetScale().x,
					0.05f);

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	// Creating a sub section for the cameras.
	if (ImGui::TreeNode("Cameras"))
	{
		// Looping through the entities in the list.s
		for (unsigned int i = 0; i < m_lCameras.size(); i++)
		{
			// Interface naming.
			std::string sInterface = "Camera " + std::to_string(i);
			sInterface += "##";
			sInterface += std::to_string(i);

			// Creating the button to swap to other Cameras.
			if (ImGui::Button(sInterface.c_str()))
			{
				m_pActiveCamera = m_lCameras[i];
			}
		}
		ImGui::TreePop();
	}

	// Closing the sub window.
	ImGui::End();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Clear the back buffer (erase what's on screen) and depth buffer
	Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), m_fBackgroundColor);
	Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Rendering the entities.
	for (unsigned int i = 0; i < m_lEntities.size(); i++)
	{
		m_lEntities[i].Draw(m_pActiveCamera, totalTime);
	}

	// Rendering ImGui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Present at the end of the frame
	bool vsync = Graphics::VsyncState();
	Graphics::SwapChain->Present(
		vsync ? 1 : 0,
		vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

	// Re-bind back buffer and depth buffer after presenting
	Graphics::Context->OMSetRenderTargets(
		1,
		Graphics::BackBufferRTV.GetAddressOf(),
		Graphics::DepthBufferDSV.Get());
}



