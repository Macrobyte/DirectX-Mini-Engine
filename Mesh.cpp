#include "Mesh.h"

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	immediateDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateDeviceContext->Draw(vertexCount, 0);
}

bool Mesh::CreateVertexBuffer()
{
	vertexCount = objRef->positionIndex.size();

	vertices = new Vertex[vertexCount];

	for (size_t i = 0; i < vertexCount; i++)
	{
		int vIndex = objRef->positionIndex[i] - 1; // Use -1 for indices as obj file indices start at 1

		// Set color data
		vertices[i].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// Set position data
		vertices[i].position.x = objRef->positionList[vIndex].x;
		vertices[i].position.y = objRef->positionList[vIndex].y;
		vertices[i].position.z = objRef->positionList[vIndex].z;

		if(objRef->texCoordIndex.size() > 0)
		{
			int tIndex = objRef->texCoordIndex[i] - 1;

			// Set texture data
			vertices[i].texCoord.x = objRef->texCoordList[tIndex].x;
			vertices[i].texCoord.y = objRef->texCoordList[tIndex].y;
		}

		if(objRef->normalIndex.size() > 0)
		{
			int nIndex = objRef->normalIndex[i] - 1;
			// Set normal data
			vertices[i].normal.x = objRef->normalList[nIndex].x;
			vertices[i].normal.y = objRef->normalList[nIndex].y;
			vertices[i].normal.z = objRef->normalList[nIndex].z;
		}
	}

	// Set up and create vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// Used by CPU and GPU
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;	// Total size of buffer, in bytes
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		// Use as a vertex buffer
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// Allow CPU to write in buffer

	if (FAILED(D3DDevice->CreateBuffer(&vertexBufferDesc, nullptr, &vertexBuffer)))
		return false;

	// Copy vertices into vertex buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	immediateDeviceContext->Map(vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);	// Map the buffer. Lock it so no one else can use it
	memcpy(ms.pData, vertices, sizeof(Vertex) * vertexCount);													// Copy the data
	immediateDeviceContext->Unmap(vertexBuffer, NULL);													// Unlock the buffer

	return true;

}
