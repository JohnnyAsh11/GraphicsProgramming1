#include "Mesh.h"

#include "Graphics.h"

using namespace DirectX;

Mesh::Mesh() {}

Mesh::Mesh(Vertex* a_pVertices, int a_dVertexCount, unsigned int* a_pIndices, int a_dIndexCount)
{
	// Saving the passed in values to the member fields.
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;

	m_dVertexCount = a_dVertexCount;
	m_dIndexCount = a_dIndexCount;

	// Setting up the vertex buffer setup struct object.
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;						// Will NEVER change
	vbd.ByteWidth = sizeof(Vertex) * a_dVertexCount;		// 3 = number of vertices in the buffer
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// Tells Direct3D this is a vertex buffer
	vbd.CPUAccessFlags = 0;									// Note: We cannot access the data from C++ (this is good)
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Setting up the index buffer setup struct object.
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;						// Will NEVER change
	ibd.ByteWidth = sizeof(unsigned int) * a_dIndexCount;	// 3 = number of indices in the buffer
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;				// Tells Direct3D this is an index buffer
	ibd.CPUAccessFlags = 0;									// Note: We cannot access the data from C++ (this is good)
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Creating the structs that actually hold the buffer data
	D3D11_SUBRESOURCE_DATA initialVertexData = {};
	D3D11_SUBRESOURCE_DATA initialIndexData = {};

	// Setting the system memory to hold the buffer data.
	initialVertexData.pSysMem = a_pVertices;
	initialIndexData.pSysMem = a_pIndices;

	// Creating the buffers.
	Graphics::Device->CreateBuffer(&vbd, &initialVertexData, m_pVertexBuffer.GetAddressOf());
	Graphics::Device->CreateBuffer(&ibd, &initialIndexData, m_pIndexBuffer.GetAddressOf());
}

#pragma region Rule of Three
Mesh::~Mesh() 
{
	// Freeing memory.
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();
}
Mesh::Mesh(const Mesh& a_pOther) 
{
	// Setting values.
	m_pVertexBuffer = a_pOther.m_pVertexBuffer;
	m_dVertexCount = a_pOther.m_dVertexCount;
	m_pIndexBuffer = a_pOther.m_pIndexBuffer;
	m_dIndexCount = a_pOther.m_dIndexCount;
}
Mesh& Mesh::operator=(const Mesh& a_pOther)
{
	// Freeing the memory in the ComPtrs.
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// Setting values.
	m_pVertexBuffer = a_pOther.m_pVertexBuffer;
	m_dVertexCount = a_pOther.m_dVertexCount;
	m_pIndexBuffer = a_pOther.m_pIndexBuffer;
	m_dIndexCount = a_pOther.m_dIndexCount;

	return *this;
}
#pragma endregion

#pragma region Accessors
BufferPtr Mesh::GetVertexBuffer(void)
{
	return m_pVertexBuffer;
}
BufferPtr Mesh::GetIndexBuffer(void)
{
	return m_pIndexBuffer;
}
int Mesh::GetIndexCount(void)
{
	return m_dIndexCount;
}
int Mesh::GetVertexCount(void)
{
	return m_dVertexCount;
}
#pragma endregion

void Mesh::Draw(void)
{
	// Setting the stride to be the memory size of a Vertex
	UINT stride = sizeof(Vertex);

	// The offset starts at the first piece of data.
	UINT offset = 0;

	// Setting the buffers as the next thing to draw.
	Graphics::Context->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	Graphics::Context->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Starting up the render pipeline and drawing the currently set Index and Vertex buffers.
	Graphics::Context->DrawIndexed(
		m_dIndexCount,		// The number of indices to use (we could draw a subset if we wanted).
		0,					// Offset to the first index we want to use.
		0);					// Offset to add to each index when looking up vertices.
}
