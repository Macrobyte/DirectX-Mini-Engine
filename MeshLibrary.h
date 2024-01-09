#pragma once
#include <map>
#include <string>

class Mesh;
class MeshLibrary
{
private:
	static std::map<std::string, Mesh*> meshes;
	static Mesh* cube;
	static Mesh* sphere;
	static Mesh* scream;
	static Mesh* plane;
	static Mesh* gravityGun;
	static Mesh* gun;

public:
	static void LoadAllMeshes();
	static void ReleaseAllMeshes();
	static Mesh* GetMesh(std::string name) { return meshes[name]; };

};

