#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Vector4 lightColor, Vector3 direction) 
	: Light(lightColor)
{
	this->direction = direction;
}


