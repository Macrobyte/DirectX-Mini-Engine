#include "BoundingBox.h"
#include "../../Core/CollisionHandler.h"
#include "Transform.h"

BoundingBox::BoundingBox()
{
	transform = Transform();
	CollisionHandler::AddBoundingBox(this);
}

BoundingBox::~BoundingBox()
{
	CollisionHandler::RemoveBoundingBox(this);
}

void BoundingBox::OnCollision(BoundingBox* other)
{
	if (onCollideCallback != nullptr)
		onCollideCallback(other);
}

bool BoundingBox::IsColliding(BoundingBox* other)
{
	//Vector3 otherMin = other->GetMin();
	//Vector3 otherMax = other->GetMax();

	//Vector3 thisMin = GetMin();
	//Vector3 thisMax = GetMax();

	//bool x = (thisMax.x >= otherMin.x) && (thisMin.x <= otherMax.x);
	//bool y = (thisMax.y >= otherMin.y) && (thisMin.y <= otherMax.y);
	//bool z = (thisMax.z >= otherMin.z) && (thisMin.z <= otherMax.z);

	//return x && y && z;

	Transform& t1 = transform;
	Transform& t2 = other->transform;

	Vector3 t1min = t1.GetPosition() - t1.GetScale();
	Vector3 t1max = t1.GetPosition() + t1.GetScale();

	Vector3 t2min = t2.GetPosition() - t2.GetScale();
	Vector3 t2max = t2.GetPosition() + t2.GetScale();

	if(t1min.x > t2max.x || t1max.x < t2min.x)
		return false;

	if(t1min.y > t2max.y || t1max.y < t2min.y)
		return false;

	if(t1min.z > t2max.z || t1max.z < t2min.z)
		return false;

	return true;
}


