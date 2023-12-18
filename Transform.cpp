#include "Transform.h"

Transform::Transform()
{
	position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale =	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
}

//DirectX::XMMATRIX Transform::GetWorldMatrix()
//{
//	const DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
//	const DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z);
//	const DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
//
//	const DirectX::XMMATRIX world = scale * rotation * translation;
//
//	return world;
//}

