#include "Transform.h"

Transform::Transform()
{
	position = Vector3(0.0f, 0.0f, 0.0f);
	rotation = Quaternion::Identity;
	scale = Vector3(1.0f, 1.0f, 1.0f);
}

Matrix Transform::GetWorldMatrix()
{
	Matrix translation = DirectX::XMMatrixTranslationFromVector(position);
	Matrix rotation = DirectX::XMMatrixRotationQuaternion(this->rotation);
	Matrix scale = DirectX::XMMatrixScalingFromVector(this->scale);

	return scale * rotation * translation;
}

Vector3 Transform::GetForwardVector()
{
	return Vector3::Transform(Vector3::Forward, rotation);
}

Vector3 Transform::GetRightVector()
{
	return Vector3::Transform(Vector3::Right, rotation);
}

Vector3 Transform::GetUpVector()
{
	return Vector3::Transform(Vector3::Up, rotation);
}

void Transform::Translate(Vector3 translation)
{
	position += translation;
}



