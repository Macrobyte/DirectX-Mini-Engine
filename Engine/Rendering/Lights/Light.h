#pragma once
#include "SimpleMath.h"

class Light
{
private:	
	DirectX::SimpleMath::Vector4 lightColor; // 16 bytes

public:
	Light() {};
	Light(DirectX::SimpleMath::Vector4 lightColor);
	~Light() {};

	DirectX::SimpleMath::Vector4 GetColor() { return lightColor; }
	void SetColor(DirectX::SimpleMath::Vector4 lightColor) { this->lightColor = lightColor; }
};

