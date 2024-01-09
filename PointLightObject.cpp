#include "PointLightObject.h"

PointLightObject::PointLightObject(Mesh* mesh, Material* material, PointLight* pointLight) 
	: GameObject(mesh, material)
{
	this->pointLight = pointLight;

	DirectX::SimpleMath::Vector4 offset = DirectX::SimpleMath::Vector4(0.0f, 0.01f, 0.0f, 1.0f);

	pointLight->SetPosition(DirectX::SimpleMath::Vector4(GetTransform()->GetPosition() + offset));

	SetTag(T_Light);
}

void PointLightObject::BeginPlay()
{
}

void PointLightObject::Update(float deltaTime)
{
	DirectX::SimpleMath::Vector4 offset = DirectX::SimpleMath::Vector4(0.0f, 0.01f, 0.0f, 1.0f); 

	pointLight->SetPosition(DirectX::SimpleMath::Vector4(GetTransform()->GetPosition() + offset));

}
