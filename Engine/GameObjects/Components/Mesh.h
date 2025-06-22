#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "../../Utility/ObjFileLoader.h"

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
	Mesh(Obj* obj, ID3D11Device* dev, ID3D11DeviceContext* devCon);
	~Mesh();

	void Draw();

private:

	Obj* objRef;

	ID3D11Device* D3DDevice;
	ID3D11DeviceContext* immediateDeviceContext;

	Vertex* vertices;
	unsigned int vertexCount;

	ID3D11Buffer* vertexBuffer;
	
	bool CreateVertexBuffer();
	void SetObj(Obj* obj) { objRef = obj; }
};

