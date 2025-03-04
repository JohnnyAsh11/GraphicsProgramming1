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
	// Initializing the Camera.
	m_lCameras.push_back(std::shared_ptr<Camera>(new Camera(Window::AspectRatio(), XMFLOAT3(0.0f, 0.0f, -5.0f), 60.0f)));
	m_lCameras.push_back(std::shared_ptr<Camera>(new Camera(Window::AspectRatio(), XMFLOAT3(2.5f, 0.0f, -5.0f), 45.0f)));
	m_lCameras.push_back(std::shared_ptr<Camera>(new Camera(Window::AspectRatio(), XMFLOAT3(-2.5f, 0.0f, -5.0f), 75.0f)));
	m_pActiveCamera = m_lCameras[0];

#pragma region Mesh Creation
	// Allocating the memory for both buffers.  Also setting the index values.
	Vertex* vertices = new Vertex[36];
	unsigned int* indices = new unsigned int[36];
	for (int i = 0; i < 36; i++) indices[i] = i;

	// Setting some variables for creating the circle.
	int dSubsivisions = 12;
	float fDeltaAngle = (2.0f * 3.14159265358979323846f) / dSubsivisions;
	float fRadius = 0.25f;
	XMFLOAT3 fOrigin = XMFLOAT3(-0.6f, +0.0f, +0.0f);
	std::vector<XMFLOAT3> vertexPositions;

	// Creating the vertices.
	for (int i = 0; i < dSubsivisions; i++)
	{
		vertexPositions.push_back(XMFLOAT3(
			static_cast<float>(cos(fDeltaAngle * i) * fRadius + fOrigin.x),
			static_cast<float>(sin(fDeltaAngle * i) * fRadius + fOrigin.y),
			0.0f));
	}

	// Populating the Vertex Buffer with the proper order of vertices.
	for (int i = 0; i < dSubsivisions; i++)
	{
		int j = i * 3;
		vertices[j] = { fOrigin, YELLOW };
		vertices[j + 2] = { vertexPositions[i], BLUE };
		vertices[j + 1] = { vertexPositions[(i + 1) % dSubsivisions], BLUE };
	}

	// Instantiating the mesh object.
	Mesh* mesh1 = new Mesh(vertices, 36, indices, 36);

	delete[] vertices;
	delete[] indices;

	// Setting the indices for the square.
	indices = new unsigned int[6];
	for (int i = 0; i < 6; i++) indices[i] = i;

	// Setting the vertices for the square.
	vertices = new Vertex[6];
	vertices[0] = { XMFLOAT3(+0.9f, +0.25f, +0.0f), PURPLE };		// Center
	vertices[1] = { XMFLOAT3(+0.9f, -0.5f, +0.0f), ORANGE };		// Bottom Right
	vertices[2] = { XMFLOAT3(+0.5f, -0.5f, +0.0f), ORANGE };		// Bottom Left

	vertices[3] = { XMFLOAT3(+0.5f, -0.5f, +0.0f), ORANGE };		// Center
	vertices[5] = { XMFLOAT3(+0.9f, +0.25f, +0.0f), PURPLE };		// Top Right
	vertices[4] = { XMFLOAT3(+0.5f, +0.25f, +0.0f), PURPLE };		// Top Left

	// Instantiating the square mesh.
	Mesh* mesh2 = new Mesh(vertices, 6, indices, 6);

	delete[] vertices;
	delete[] indices;
#pragma endregion

	std::shared_ptr<SimpleVertexShader> pVS = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> pPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());

	Material* mat1 = new Material(pVS, pPS, XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));
	Material* mat2 = new Material(pVS, pPS, XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));
	Material* mat3 = new Material(pVS, pPS, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));

	for (unsigned int i = 0; i < 4; i++)
	{
		m_lEntities.push_back(Entity(mesh1, mat1));
	}
	m_lEntities.push_back(Entity(mesh2, mat3));
	//m_lEntities[1].SetMaterial(mat2);

	delete mesh1, mesh2;
	delete mat1, mat2, mat3;
	
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
	Transform& current = m_lEntities[0].GetTransform();
	current.SetPosition(static_cast<float>(sin(totalTime)), 0.0f, 0.0f);

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
		m_lEntities[i].Draw(m_pActiveCamera);
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



