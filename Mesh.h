#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "ObjFileModel.h"

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;
};

//TODO - Figure out meshes
class Mesh
{
public:
	Mesh();
	~Mesh();

	void Draw();

private:

	Obj* objFileModel;

	ID3D11Device* D3DDevice;
	ID3D11DeviceContext* immediateDeviceContext;

	std::vector<Vertex> vertices;
	unsigned int vertexCount;

	ID3D11Buffer* vertexBuffer;
	
	bool CreateVertexBuffer();
};

