#include "MaterialLibrary.h"
#include "Material.h"


std::map<std::string, Material*> MaterialLibrary::materials;
Material* MaterialLibrary::nullMaterial;
Material* MaterialLibrary::skyboxMaterial;
Material* MaterialLibrary::skyboxNightMaterial;
Material* MaterialLibrary::reflectiveMaterial;
Material* MaterialLibrary::brownMaterial;
Material* MaterialLibrary::checkeredMaterial;
Material* MaterialLibrary::lightMaterial;
Material* MaterialLibrary::blueMaterial;
Material* MaterialLibrary::metalMaterial;
Material* MaterialLibrary::gunMaterial;



void MaterialLibrary::LoadAllMaterials()
{
	nullMaterial = new Material(Renderer::GetInstance().GetDevice(), L"Compiled Shaders/VertexShader.cso", L"Compiled Shaders/PixelShader.cso", L"Textures/Null.png", CB_Lit);
	materials["null"] = nullMaterial;

	skyboxMaterial = new Material(Renderer::GetInstance().GetDevice(), L"Compiled Shaders/SkyboxVShader.cso", L"Compiled Shaders/SkyboxPShader.cso", L"Textures/Skybox.dds", CB_Unlit, true);
	materials["skyday"] = skyboxMaterial;

	reflectiveMaterial = new Material(Renderer::GetInstance().GetDevice(), L"Compiled Shaders/ReflectiveVShader.cso", L"Compiled Shaders/ReflectivePShader.cso", L"Textures/Gray.png", CB_Reflective, L"Textures/Skybox.dds");
	materials["reflective"] = reflectiveMaterial;

	brownMaterial = new Material(Renderer::GetInstance().GetDevice(), L"Compiled Shaders/VertexShader.cso", L"Compiled Shaders/PixelShader.cso", L"Textures/Box.bmp", CB_Lit);
	materials["brown"] = brownMaterial;

	checkeredMaterial = new Material(Renderer::GetInstance().GetDevice(), L"Compiled Shaders/VertexShader.cso", L"Compiled Shaders/PixelShader.cso", L"Textures/Checkered.jpg", CB_Lit);
	materials["checkered"] = checkeredMaterial;

	lightMaterial = new Material(Renderer::GetInstance().GetDevice(), L"Compiled Shaders/VertexShader.cso", L"Compiled Shaders/PixelShader.cso", L"Textures/White.png", CB_Lit);
	materials["light"] = lightMaterial;

	blueMaterial = new Material(Renderer::GetInstance().GetDevice(), L"Compiled Shaders/VertexShader.cso", L"Compiled Shaders/PixelShader.cso", L"Textures/Blue.png", CB_Lit);
	materials["blue"] = blueMaterial;

	metalMaterial = new Material(Renderer::GetInstance().GetDevice(), L"Compiled Shaders/VertexShader.cso", L"Compiled Shaders/PixelShader.cso", L"Textures/metal.jpg", CB_Lit);
	materials["metal"] = metalMaterial;

	skyboxNightMaterial = new Material(Renderer::GetInstance().GetDevice(), L"Compiled Shaders/SkyboxVShader.cso", L"Compiled Shaders/SkyboxPShader.cso", L"Textures/SkyboxNight.dds", CB_Unlit, true);
	materials["skynight"] = skyboxNightMaterial;

	gunMaterial = new Material(Renderer::GetInstance().GetDevice(), L"Compiled Shaders/VertexShader.cso", L"Compiled Shaders/PixelShader.cso", L"Textures/noodas.png", CB_Lit);
	materials["gun"] = gunMaterial;

}

void MaterialLibrary::ReleaseAllMaterials()
{
	for (auto& material : materials)
	{
		delete material.second;
	}
}
