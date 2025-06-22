#pragma once
#include <vector>
#include "../GameObjects/Components/BoundingBox.h"

struct Ray
{
	Vector3 origin;
	Vector3 direction;
};

class CollisionHandler
{
private:
	static std::vector<BoundingBox*> boundingBoxes;

public:
	static void AddBoundingBox(BoundingBox* box);
	static void RemoveBoundingBox(BoundingBox* box);


	static void Update();
};

