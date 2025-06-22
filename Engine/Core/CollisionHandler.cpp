#include "CollisionHandler.h"
#include <iostream>

std::vector<BoundingBox*> CollisionHandler::boundingBoxes;

void CollisionHandler::AddBoundingBox(BoundingBox* box)
{
	std::cout << "Adding bounding box" << std::endl;
	boundingBoxes.push_back(box);
}

void CollisionHandler::RemoveBoundingBox(BoundingBox* box)
{
	std::cout << "Removing bounding box" << std::endl;
	
	for (int i = 0; i < boundingBoxes.size(); i++)
	{
		if (boundingBoxes[i] == box)
		{
			boundingBoxes.erase(boundingBoxes.begin() + i);
			return;
		}
	}
}


void CollisionHandler::Update()
{
	for (int i = 0; i < boundingBoxes.size(); i++)
	{
		for (int j = i + 1; j < boundingBoxes.size(); j++)
		{
			if (boundingBoxes[i]->IsColliding(boundingBoxes[j]))
			{
				boundingBoxes[i]->OnCollision(boundingBoxes[j]);
				boundingBoxes[j]->OnCollision(boundingBoxes[i]);
			}
		}
	}

}
