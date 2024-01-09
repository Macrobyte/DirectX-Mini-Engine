#include "Collectible.h"
#include "BoundingBox.h"
#include "Game.h"
#include "SoundHandler.h"

Collectible::Collectible(Mesh* mesh, Material* material)
	: GameObject(mesh, material)
{
	SetTag(T_Collectible);
	SetName("Collectible");
	GenerateRandomPosition(-30.0f, 40.0f, 0.11f, 0.10f, -30.0f, 40.0f);
	SetAnimated(true);
	SetScale({ 2.0f,2.0f,2.0f });
}

Collectible::~Collectible()
{
	GameObject::~GameObject();
}

void Collectible::BeginPlay()
{
	GameObject::BeginPlay();
}

void Collectible::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
}

void Collectible::OnCollision(BoundingBox* other)
{
	if (other->GetGameObject()->GetTag() == T_Player)
	{
		SoundHandler::GetInstance().PlayOneShot("Scream");

		Game::GetInstance().AddScore(score);

		MarkForRemoval();
	}
}
