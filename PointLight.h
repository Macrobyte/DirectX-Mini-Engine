#pragma once
#include "Light.h"
#include "Transform.h"

#define MAX_POINT_LIGHTS 8

class PointLight : public Light
{
private:
	DirectX::SimpleMath::Vector4 position ; // 16 bytes
	float strength; // 4 bytes
	BOOL enabled; // 4 bytes
	float padding[2]; // 8 bytes

public:
	PointLight() {};
	PointLight(DirectX::SimpleMath::Vector4 lightColor, DirectX::SimpleMath::Vector3 position, float strength, BOOL enabled = false);
	~PointLight() {};

	DirectX::SimpleMath::Vector4 GetPosition() { return position; }

	float GetStrength() { return strength; }
	BOOL IsEnabled() { return enabled; }

	void SetPosition(DirectX::SimpleMath::Vector4 position) { this->position = position; }
	void SetStrength(float strength) { this->strength = strength; }
	void SetEnabled(BOOL enabled) { this->enabled = enabled; }
};

