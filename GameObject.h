#pragma once
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "./Engine/Utility/TypeHelpers.h"


enum Tag
{
	T_Default,
	T_Player,
	T_Ground,
	T_Collectible,
	T_TargetBox,
	T_Bullet,
};

class BoundingBox;

class GameObject
{
private:

	Transform transform;
	Mesh* mesh = nullptr;
	Material* material = nullptr;

	Name name;
	Tag tag;

	BoundingBox* boundingBox = nullptr;
	bool collided = false;

	bool isMarkedForRemoval = false;

	bool isAnimated = false;

public:
	GameObject(Mesh* mesh, Material* material);
	~GameObject();
	
	virtual void BeginPlay();
	virtual void Update(float deltaTime);


	virtual void OnCollision(BoundingBox* other);

	void Animate(float deltaTime);

	Transform& GetTransform() { return transform; }

	Quaternion& GetRotation() { return transform.GetRotation(); }
	void SetRotation(const Quaternion& rotation) { transform.SetRotation(rotation); }

	Vector3& GetPosition() { return transform.GetPosition(); }
	void SetPosition(const Vector3& position) { transform.SetPosition(position); }


	Vector3& GetScale() { return transform.GetScale(); }
	void SetScale(const Vector3& scale) { transform.SetScale(scale); }


	Mesh* GetMesh() { return mesh; }
	Material* GetMaterial() { return material; }

	Name& GetName() { return name; }
	void SetName(const Name& name) { this->name = name; }

	Tag& GetTag() { return tag; }
	void SetTag(const Tag& tag) { this->tag = tag; }

	BoundingBox* GetBoundingBox() { return boundingBox; }

	bool IsMarkedForRemoval() { return isMarkedForRemoval; }
	void MarkForRemoval() { isMarkedForRemoval = true; }

	bool IsAnimated() { return isAnimated; }
	void SetAnimated(bool animated) { isAnimated = animated; }

	bool IsCollided() { return collided; }
	void SetCollided(bool collided) { this->collided = collided; }

	void GenerateRandomPosition(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);

};

