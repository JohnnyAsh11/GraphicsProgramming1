#ifndef __MESH_H_
#define __MESH_H_

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"

typedef Microsoft::WRL::ComPtr<ID3D11Buffer> BufferPtr;

/// <summary>
/// Manages Vertex/Index Buffer objects for rendering to the window.
/// </summary>
class Mesh
{
private:
	BufferPtr m_pVertexBuffer;
	BufferPtr m_pIndexBuffer;
	int m_dIndexCount;
	int m_dVertexCount;

public:
	
	// Construction / Rule of Three:
	/// <summary>
	/// Constructs an instance of the Mesh class.
	/// </summary>
	/// <param name="a_pVertices">The vertices inside of this instance of a Mesh object.</param>
	/// <param name="a_dVertexCount">The quantity of vertices in the array.</param>
	/// <param name="a_pIndices">The indices inside of this instance of a Mesh object.</param>
	/// <param name="a_dIndexCount">The amount of indices in the array.</param>
	Mesh(Vertex* a_pVertices, int a_dVertexCount, unsigned int* a_pIndices, int a_dIndexCount);

	#pragma region Rule of Three
	/// <summary>
	/// Destructs instances of the Mesh object.
	/// </summary>
	~Mesh();
	/// <summary>
	/// Copy constructor instantiates a copy of a passed in mesh object.
	/// </summary>
	/// <param name="a_pOther">The mesh object that this one is copying.</param>
	Mesh(const Mesh& a_pOther);
	/// <summary>
	/// Copy operator deep copies data from the assigned mesh object.
	/// </summary>
	/// <param name="a_pOther">The mesh that is having its data copied.</param>
	/// <returns>The new copy.</returns>
	Mesh& operator= (const Mesh& a_pOther);
	#pragma endregion

	// Accessors:
	/// <summary>
	/// Retrieves the Vertex Buffer ComPtr.
	/// </summary>
	/// <returns>The Vertex Buffer ptr.</returns>
	BufferPtr GetVertexBuffer(void);

	/// <summary>
	/// Retrieves the Index Buffer ComPtr.
	/// </summary>
	/// <returns>The Index Buffer ptr.</returns>
	BufferPtr GetIndexBuffer(void);

	/// <summary>
	/// Retrieves the amount of indices in the Index Buffer.
	/// </summary>
	/// <returns>The amount of indices in the Index Buffer.</returns>
	int GetIndexCount(void);

	/// <summary>
	/// Retrieves the amount of vertices in the Vertex Buffer.
	/// </summary>
	/// <returns>The amount of vertices in the Vertex Buffer.</returns>
	int GetVertexCount(void);

	// Functional Methods:
	/// <summary>
	/// Sets the buffers and draws with the proper amount of indices.
	/// </summary>
	void Draw(void);
};

#endif //__MESH_H_
