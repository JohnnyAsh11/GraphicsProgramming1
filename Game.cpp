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

#include "WICTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;

void Game::Initialize()
{
	// Seeding the random object.
	srand(static_cast<unsigned int>(time(0)));

	// Initializing the Camera.
	m_lCameras.push_back(std::shared_ptr<Camera>(new Camera(Window::AspectRatio(), XMFLOAT3(0.0f, 0.0f, -5.0f), 60.0f)));
	m_pActiveCamera = m_lCameras[0];

	// Loading in the textures:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pCobblestoneTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pCobblestoneNormalMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pCushionTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pCushionNormalMap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		L"Textures/cobblestone.png",
		nullptr,
		&pCobblestoneTexture);
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		L"Textures/cushion.png",
		nullptr,
		&pCushionTexture);

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		L"Textures/cobblestone_normals.png",
		nullptr,
		&pCobblestoneNormalMap);
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		L"Textures/cushion_normals.png",
		nullptr,
		&pCushionNormalMap);

	// Creating the SampleState.
	D3D11_SAMPLER_DESC sampleDesc;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampleDesc.MaxAnisotropy = 8;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampleDesc.MipLODBias = 0;
	sampleDesc.MinLOD = 0;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	Graphics::Device.Get()->CreateSamplerState(&sampleDesc, &pSampler);

	std::shared_ptr<SimpleVertexShader> pBasicVS = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> pBasicPS = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());

	// Creating the materials.
	std::shared_ptr<Material> matCobblestone = 
		std::make_shared<Material>(Material(pBasicVS, pBasicPS, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.9f));
	std::shared_ptr<Material> matCushion = 
		std::make_shared<Material>(Material(pBasicVS, pBasicPS, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f));

	// Providing the materials with their respective textures.
	matCushion->AddSampler("BasicSampler", pSampler);
	matCushion->AddTexturesSRV("SurfaceTexture", pCushionTexture);
	matCushion->AddTexturesSRV("NormalMap", pCushionNormalMap);
	matCushion->SetScale(DirectX::XMFLOAT2(3.0f, 3.0f));

	matCobblestone->AddSampler("BasicSampler", pSampler);
	matCobblestone->AddTexturesSRV("SurfaceTexture", pCobblestoneTexture);
	matCobblestone->AddTexturesSRV("NormalMap", pCobblestoneNormalMap);
	matCobblestone->SetScale(DirectX::XMFLOAT2(1.0f, 1.0f));

	// Creating the 3D models.
	std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(Mesh("Models/cube.graphics_obj"));
	std::shared_ptr<Mesh> cylinder = std::make_shared<Mesh>(Mesh("Models/cylinder.graphics_obj"));
	std::shared_ptr<Mesh> sphere = std::make_shared<Mesh>(Mesh("Models/sphere.graphics_obj"));
	std::shared_ptr<Mesh> helix = std::make_shared<Mesh>(Mesh("Models/helix.graphics_obj"));
	std::shared_ptr<Mesh> torus = std::make_shared<Mesh>(Mesh("Models/torus.graphics_obj"));
	std::shared_ptr<Mesh> quad = std::make_shared<Mesh>(Mesh("Models/quad.graphics_obj"));
	std::shared_ptr<Mesh> quadDoubleSided = std::make_shared<Mesh>(Mesh("Models/quad_double_sided.graphics_obj"));

	// Controls the amount of sets of Entities are created.
	int dAmountOfSets = 2;

	// Instantiating the Entities.
	for (int i = 0; i < dAmountOfSets; i++)
	{
		m_lEntities.push_back(Entity(cube, matCobblestone));
		m_lEntities.push_back(Entity(cylinder, matCobblestone));
		m_lEntities.push_back(Entity(sphere, matCobblestone));
		m_lEntities.push_back(Entity(helix, matCobblestone));
		m_lEntities.push_back(Entity(torus, matCobblestone));
		m_lEntities.push_back(Entity(quadDoubleSided, matCobblestone));
		m_lEntities.push_back(Entity(quad, matCobblestone));
	}

	for (int j = 0; j < dAmountOfSets; j++)
	{
		for (int i = 0; i < m_lEntities.size() / dAmountOfSets; i++)
		{
			int index = i + (j * (static_cast<int>(m_lEntities.size()) / dAmountOfSets));

			// Setting the scale and getting the current Transform.
			float fUniformScale = 0.25f;
			Transform& current = m_lEntities[index].GetTransform();

			// Setting the scale and spacing out the models.
			current.SetScale(fUniformScale, fUniformScale, fUniformScale);
			current.SetPosition((i * 1.25f) - 4.0f, -1.0f + j, 0.0f);
			current.Rotate(XMFLOAT3(0.0f, 2.6f, 0.0f));

			// Setting the proper materials.
			if (j == 0)
			{
				m_lEntities[index].SetMaterial(matCobblestone);
			}
			else if (j == 1)
			{
				m_lEntities[index].SetMaterial(matCushion);
			}
		}
	}

	// Creating the lights.
	for (int i = 0; i < 5; i++)
	{
		Light currentLight = {};

		// Red light coming from the left.
		if (i == 0)
		{
			currentLight.Type = LIGHT_TYPE_DIRECTIONAL;
			currentLight.Intensity = 0.6f;
			currentLight.Direction = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
			currentLight.Color = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		}
		// Green light coming from above.
		else if (i == 1)
		{
			currentLight.Type = LIGHT_TYPE_DIRECTIONAL;
			currentLight.Intensity = 0.6f;
			currentLight.Direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
			currentLight.Color = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		}
		// Blue light coming from the right.
		else if (i == 2)
		{
			currentLight.Type = LIGHT_TYPE_DIRECTIONAL;
			currentLight.Intensity = 0.6f;
			currentLight.Direction = DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f);
			currentLight.Color = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		}
		// Creating a point light.
		else if (i == 3)
		{
			currentLight.Type = LIGHT_TYPE_POINT;
			currentLight.Intensity = 2.0f;
			currentLight.Direction = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
			currentLight.Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
			currentLight.Position = DirectX::XMFLOAT3(0.25f, -1.0f, 0.0f);
			currentLight.Range = 4.0f;
		}
		// Creating a spot light.
		else if (i == 4)
		{
			currentLight.Type = LIGHT_TYPE_SPOT;
			currentLight.Intensity = 2.5f;
			currentLight.Direction = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			currentLight.Color = DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f);
			currentLight.Position = DirectX::XMFLOAT3(2.85f, 0.5f, 0.0f);
			currentLight.SpotInnerAngle = DirectX::XMConvertToRadians(45.0f);
			currentLight.SpotOuterAngle = DirectX::XMConvertToRadians(60.0f);
			currentLight.Range = 2.0f;
		}

		m_lLights.push_back(currentLight);
	}

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

	// Making the meshes slowly rotate.
	for (unsigned int i = 0; i < m_lEntities.size(); i++)
	{
		m_lEntities[i].GetTransform().Rotate(XMFLOAT3(0.0f, deltaTime / 4, 0.0f));
	}

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
				if (ImGui::TreeNode("Material Textures:"))
				{
					// Looping through the textures in the entity's map,
					for (const auto& t : m_lEntities[i].GetMaterial().get()->GetTextures())
					{
						// And displaying them in the UI.
						//		Casting to a void ptr then a ImTextureID.
						void* p = t.second.Get();
						ImGui::Image((ImTextureID)p, ImVec2(200, 200));
					}
					ImGui::TreePop();
				}
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
	// Creating a sub section for the Lights.
	if (ImGui::TreeNode("Lights"))
	{
		// Looping through the entities in the list.s
		for (unsigned int i = 0; i < m_lLights.size(); i++)
		{
			// Interface naming.
			std::string sInterface = "Light " + std::to_string(i);
			sInterface += "##";
			sInterface += std::to_string(i);
			
			if (ImGui::TreeNode(sInterface.c_str()))
			{
				Light current = m_lLights[i];
				ImGui::DragFloat3(
					("Position##" + std::to_string(i)).c_str(),
					&current.Position.x,
					0.05f);
				ImGui::DragFloat3(
					("Color##" + std::to_string(i)).c_str(),
					&current.Color.x,
					0.05f);
				ImGui::DragFloat(
					("Intensity##" + std::to_string(i)).c_str(),
					&current.Intensity,
					0.05f);
				m_lLights[i] = current;
				ImGui::TreePop();
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
		// Setting the ambient light.
		m_lEntities[i].GetMaterial()->GetPixelShader()->SetFloat3("ambient", m_v3AmbientColor);

		// Passing in light data.
		m_lEntities[i].GetMaterial()->GetPixelShader()->SetData("lights", &m_lLights[0], sizeof(Light) * (int)m_lLights.size());
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



