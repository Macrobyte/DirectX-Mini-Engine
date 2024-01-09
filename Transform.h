#pragma once
#include <DirectXMath.h>
#include "SimpleMath.h"
#include "TypeHelpers.h"

class Transform
{
private:
	Vector3 position;
	Vector3 scale;
	Quaternion rotation;

public:
	Transform();
	~Transform() = default;

	Vector3& GetPosition() { return position; }
	Quaternion& GetRotation() { return rotation; }
	Vector3& GetScale() { return scale; }

	void SetPosition(const float& x, const float& y, const float& z) { position = Vector3(x, y, z); }
	void SetPosition(const Vector3& pos) { position = pos; }
	void SetRotation(const float& x, const float& y, const float& z) { rotation = Quaternion::CreateFromYawPitchRoll(y, x, z); }
	void SetRotation(const Quaternion& rot) { rotation = rot; }
	void SetScale(const float& x, const float& y, const float& z) { scale = Vector3(x, y, z); }
	void SetScale(const Vector3& scale) { this->scale = scale; }


	Matrix GetWorldMatrix();

	Vector3 GetForwardVector();
	Vector3 GetRightVector();
	Vector3 GetUpVector();

	void Translate(Vector3 translation);


};

