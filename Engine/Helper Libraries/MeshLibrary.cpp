#include "MeshLibrary.h"
#include "../GameObjects/Components/Mesh.h"
#include "ObjLibrary.h"
#include "../Rendering/Renderer.h"

std::map<std::string, Mesh*> MeshLibrary::meshes;
Mesh* MeshLibrary::cube;
Mesh* MeshLibrary::sphere;
Mesh* MeshLibrary::scream;
Mesh* MeshLibrary::plane;
Mesh* MeshLibrary::gravityGun;
Mesh* MeshLibrary::gun;


void MeshLibrary::LoadAllMeshes()
{
	ObjLibrary::LoadAllModels();

	cube = new Mesh(ObjLibrary::GetModel("cube"), Renderer::GetInstance().GetDevice(), Renderer::GetInstance().GetDeviceContext());
	meshes["cube"] = cube;

	sphere = new Mesh(ObjLibrary::GetModel("sphere"), Renderer::GetInstance().GetDevice(), Renderer::GetInstance().GetDeviceContext());
	meshes["sphere"] = sphere;

	scream = new Mesh(ObjLibrary::GetModel("scream"), Renderer::GetInstance().GetDevice(), Renderer::GetInstance().GetDeviceContext());
	meshes["scream"] = scream;

	plane = new Mesh(ObjLibrary::GetModel("plane"), Renderer::GetInstance().GetDevice(), Renderer::GetInstance().GetDeviceContext());
	meshes["plane"] = plane;

	gravityGun = new Mesh(ObjLibrary::GetModel("gravityGun"), Renderer::GetInstance().GetDevice(), Renderer::GetInstance().GetDeviceContext());
	meshes["gravityGun"] = gravityGun;

	gun = new Mesh(ObjLibrary::GetModel("gun"), Renderer::GetInstance().GetDevice(), Renderer::GetInstance().GetDeviceContext());
	meshes["gun"] = gun;
}

void MeshLibrary::ReleaseAllMeshes()
{
	for (auto& mesh : meshes)
	{
		delete mesh.second;
	}
}
