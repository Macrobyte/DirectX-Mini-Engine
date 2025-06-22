#pragma once
#include "../../Utility/TypeHelpers.h"
#include <functional>
#include "Transform.h"
#include <iostream>

class GameObject;
class Transform;

class BoundingBox
{
private:
	Transform transform;
	Vector3 min;
	Vector3 max;

	typedef std::function<void(BoundingBox*)> OnCollisionCallback;
	OnCollisionCallback onCollideCallback = nullptr;

	GameObject* gameObject = nullptr;


public:
	BoundingBox();
	~BoundingBox();

	void SetOnCollideCallback(OnCollisionCallback callback) { onCollideCallback = callback; }

	void OnCollision(BoundingBox* other);
	bool IsColliding(BoundingBox* other);

	Vector3& GetPosition() { return transform.GetPosition(); }
	void SetPosition(const Vector3& position) { transform.GetPosition() = position;
}

	Vector3& GetScale() { return transform.GetScale(); }
	void SetScale(const Vector3& scale) { transform.SetScale(scale); }

	Quaternion& GetRotation() { return transform.GetRotation(); }
	void SetRotation(const Quaternion& rotation) { transform.SetRotation(rotation); }

	Vector3 GetMin() { return min; }
	Vector3 GetMax() { return max; }

	void SetGameObject(GameObject* gameObject) { this->gameObject = gameObject; }
	GameObject* GetGameObject() { return gameObject; }


};

