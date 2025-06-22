#pragma once
#include "../Rendering/Renderer.h"

class Mesh;
class Material;
class Camera;	

class Skybox
{
	Material* material;
	Mesh* mesh;

public:
	Skybox(Mesh* mesh, Material* material);
	~Skybox();

	Material* GetMaterial() { return material; }
	void SetMaterial(Material* material) { this->material = material; }

	Mesh* GetMesh() { return mesh; }
	void SetMesh(Mesh* mesh) { this->mesh = mesh; }

	void Render(ID3D11DeviceContext* devcon, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);
};

