#include "TargetBox.h"
#include "./Components/BoundingBox.h"
#include "../Core/Game/Game.h"

TargetBox::TargetBox(Mesh* mesh, Material* material) 
	: GameObject(mesh, material)
{
	SetTag(T_TargetBox);
	SetName("TargetBox");

	GenerateRandomPosition(-30.0f, 40.0f, 0.11f, 0.10f, -30.0f, 40.0f);
}

TargetBox::~TargetBox()
{
	GameObject::~GameObject();
}

void TargetBox::BeginPlay()
{
	GameObject::BeginPlay();
}

void TargetBox::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
}

void TargetBox::OnCollision(BoundingBox* other)
{
	GameObject::OnCollision(other);

	if (other->GetGameObject()->GetTag() == T_Bullet)
	{
		Game::GetInstance().AddScore(score);
		MarkForRemoval();
		other->GetGameObject()->MarkForRemoval();
	}

}

