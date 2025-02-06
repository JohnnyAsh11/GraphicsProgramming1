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

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

#pragma region Meshes
	// Setting the Vertices of the first triangle.
	Vertex* vertices = new Vertex[3];
	vertices[0] = { XMFLOAT3(+0.0f, +0.25f, +0.0f), RED };
	vertices[1] = { XMFLOAT3(+0.25f, -0.25f, +0.0f), GREEN };
	vertices[2] = { XMFLOAT3(-0.25f, -0.25f, +0.0f), BLUE };
	
	// Setting the indices of the first triangle.
	unsigned int* indices = new unsigned int[3];
	for (int i = 0; i < 3; i++) indices[i] = i;
	
	// Instantiating the first mesh.
	m_mMesh1 = new Mesh(vertices, 3, indices, 3);
	
	delete[] indices;
	delete[] vertices;	

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
	m_mMesh2 = new Mesh(vertices, 6, indices, 6);
	
	delete[] vertices;
	delete[] indices;

	// Allocating the memory for both buffers.  Also setting the index values.
	vertices = new Vertex[36];
	indices = new unsigned int[36];
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
	m_mMesh3 = new Mesh(vertices, 36, indices, 36);

	delete[] vertices;
	delete[] indices;
#pragma endregion
	
	unsigned int cBufferSize = sizeof(VertexColorOffsetData);
	// Calculating the memory size in multiples of 16 by taking
	//  advantage of int division.
	cBufferSize = ((cBufferSize + 15) / 16) * 16;

	D3D11_BUFFER_DESC cbDesc{};
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = cBufferSize;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Creating the buffer with the description struct.
	Graphics::Device->CreateBuffer(&cbDesc, 0, m_pConstantBuffer.GetAddressOf());

	m_tTransform = Transform();

	// Binding the buffer to the b0 slot for use.
	Graphics::Context->VSSetConstantBuffers(
		0,
		1,
		m_pConstantBuffer.GetAddressOf());

	// Setting the starting tint values.
	m_v4MeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
	}
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	delete m_mMesh1;
	delete m_mMesh2;
	delete m_mMesh3;

	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	UpdateImGui(deltaTime);

	m_tTransform.SetPosition(sin(totalTime), 0.0f, 0.0f);
	m_v3MeshPosition = m_tTransform.GetPosition();
	m_v4MeshColor.x = sin(totalTime) * 0.5f + 0.5f;
	m_v4MeshColor.y = sin(totalTime) * 0.5f + 0.5f;
	m_v4MeshColor.z = sin(totalTime) * 0.5f + 0.5f;

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
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

	// Creating UI draggers for the color and position of the meshes.
	ImGui::DragFloat3("Mesh positions", &m_v3MeshPosition.x, 0.05f);
	ImGui::ColorEdit4("Mesh color tints", &m_v4MeshColor.x);

	// Closing the sub window.
	ImGui::End();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	m_fBackgroundColor);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// Sending data to GPU with the constant buffer.
	// 1. Collect data (Creating a data transfer object)
	VertexColorOffsetData DTO{};
	DTO.m_v4Color = m_v4MeshColor;
	DTO.m_v3Offset = m_v3MeshPosition;

	// 2. Copy to GPU with memcpy
	// Creating a mapped subresource struct to hold the cbuffer GPU address
	D3D11_MAPPED_SUBRESOURCE mapped{};

	// Actually grabbing the cbuffer's address
	Graphics::Context->Map(
		m_pConstantBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);

	// Copying the data to the GPU
	memcpy(mapped.pData, &DTO, sizeof(VertexColorOffsetData));

	// Unmapping from the memory address.
	Graphics::Context->Unmap(m_pConstantBuffer.Get(), 0);

	//---------------------------------------------------------------
	// DRAW HERE:
	
	m_mMesh1->Draw();
	m_mMesh2->Draw();
	m_mMesh3->Draw();	

	// Rendering ImGui
	ImGui::Render(); // Turns this frame’s UI into renderable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//---------------------------------------------------------------

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
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
}



