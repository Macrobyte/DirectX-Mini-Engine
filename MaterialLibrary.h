#pragma once
#include <map>
#include <string>

class Material;

class MaterialLibrary
{
private:
	static std::map<std::string, Material*> materials;
	static Material* nullMaterial;
	static Material* skyboxMaterial;
	static Material* skyboxNightMaterial;
	static Material* reflectiveMaterial;
	static Material* brownMaterial;
	static Material* checkeredMaterial;
	static Material* lightMaterial;
	static Material* blueMaterial;
	static Material* metalMaterial;

	static Material* gunMaterial;

public:
	static void LoadAllMaterials();
	static void ReleaseAllMaterials();
	static Material* GetMaterial(std::string name) { return materials[name]; };
};

