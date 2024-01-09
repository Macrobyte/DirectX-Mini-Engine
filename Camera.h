#pragma once
#include "SimpleMath.h"
#include "TypeHelpers.h"

class Camera
{
public:
	Camera() {};
	~Camera() {};

	Vector3 position = { 0,0,0 };
	Quaternion rotation = { 0,0,0,1 };

	Matrix GetViewMatrix();

	Vector3 ForwardVector();
	Vector3 RightVector();
	Vector3 UpVector();

	Vector3 GetPosition() { return position; }
	void SetPosition(Vector3 pos) { position = pos; }

	Quaternion GetRotation() { return rotation; }
	void SetRotation(Quaternion rot) { rotation = rot; }

};

