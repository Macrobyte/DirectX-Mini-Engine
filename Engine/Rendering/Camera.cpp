#include "Camera.h"

Vector3 Camera::ForwardVector()
{
	return Vector3::Transform(Vector3::Forward, rotation);
}

Vector3 Camera::RightVector()
{
	return Vector3::Transform(Vector3::Right, rotation);
}

Vector3 Camera::UpVector()
{
	return Vector3::Transform(Vector3::Up, rotation);
}

Matrix Camera::GetViewMatrix()
{
	Vector3 lookTo = ForwardVector();
	Vector3 camUp = UpVector();

	return DirectX::XMMatrixLookToLH(position, lookTo, camUp);
}
