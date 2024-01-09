#pragma once
#include "Light.h"
#include "TypeHelpers.h"

class DirectionalLight : public Light
{
private:
	Vector3 direction;

public:
	DirectionalLight(Vector4 lightColor, Vector3 direction);
	~DirectionalLight() {};

	Vector3 GetLightDirection() { return direction; }
};


