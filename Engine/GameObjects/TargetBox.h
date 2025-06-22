#pragma once
#include "GameObject.h"

class TargetBox : public GameObject
{
private:
	int score = 5;
public:
	TargetBox(Mesh* mesh, Material* material);
	~TargetBox();
	void BeginPlay() override;
	void Update(float deltaTime) override;
	void OnCollision(BoundingBox* other) override;

	int GetScore() { return score; }

};


