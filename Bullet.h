#pragma once
#include "GameObject.h"

class Bullet : public GameObject
{
private:
	float speed = 5.0f;
	float lifeTime = 5.0f;


public:
		Bullet(Mesh* mesh, Material* material, float speed, float lifeTime);
		~Bullet();
		void BeginPlay() override;
		void Update(float deltaTime) override;
		void OnCollision(BoundingBox* other) override;



};

