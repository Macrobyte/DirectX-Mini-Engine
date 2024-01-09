#pragma once
#include "GameObject.h"

class Collectible : public GameObject
{
private:
	int score = 10;

public:
	Collectible(Mesh* mesh, Material* material);
	~Collectible();
	void BeginPlay() override;
	void Update(float deltaTime) override;
	void OnCollision(BoundingBox* other) override;
	int GetScore() { return score; }


};
