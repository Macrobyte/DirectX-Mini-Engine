#pragma once
#include "GameObject.h"
#include "PointLight.h"

class PointLightObject : public GameObject
{
private:
	PointLight* pointLight;

public:
	PointLightObject(Mesh* mesh, Material* material, PointLight* pointLight);
	~PointLightObject() {};

	void BeginPlay() override;
	void Update(float deltaTime) override;	

	PointLight* GetPointLight() { return pointLight; }
};

