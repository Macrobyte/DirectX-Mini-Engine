#pragma once
#include "GameObject.h"

class Camera;

class Player : public GameObject
{
private:
	float speed = 5.0f;

	float sensitivity = DirectX::XM_2PI * 0.00025f;
	float rotationSpeed = 2.0f;

	Camera* camera = nullptr;


public:
	Player(Mesh* mesh, Material* material, Camera* camera);
	~Player();
	void BeginPlay() override;
	void Update(float deltaTime) override;
	void OnCollision(BoundingBox* other) override;

	void HandleMovement(float deltaTime);
	void HandleRotation(float deltaTime);

	void Shoot();

};

