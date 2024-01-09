#include "PointLight.h"

PointLight::PointLight(DirectX::SimpleMath::Vector4 lightColor, DirectX::SimpleMath::Vector3 position, float strength, BOOL enabled) 
	: Light(lightColor)
{
	this->position = DirectX::SimpleMath::Vector4(position);
	this->strength = strength;
	this->enabled = enabled;

	padding[0] = 0.0f;
	padding[1] = 0.0f;
}
