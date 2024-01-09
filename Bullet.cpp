#include "Bullet.h"
#include "BoundingBox.h"

Bullet::Bullet(Mesh* mesh, Material* material, float speed, float lifeTime) 
	: GameObject(mesh, material)
{
	this->speed = speed;
	this->lifeTime = lifeTime;

	SetTag(T_Bullet);
	SetName("Bullet");
}

Bullet::~Bullet()
{
	GameObject::~GameObject();
}

void Bullet::BeginPlay()
{
	GameObject::BeginPlay();
}

void Bullet::Update(float deltaTime)
{
	GameObject::Update(deltaTime);

	GetTransform().Translate(GetTransform().GetForwardVector() * speed * deltaTime);

	lifeTime -= deltaTime;
	if (lifeTime <= 0.0f)
	{
		MarkForRemoval();
	}
}

void Bullet::OnCollision(BoundingBox* other)
{
	GameObject::OnCollision(other);
	if (other->GetGameObject()->GetTag() == T_TargetBox)
	{
		std::cout << "Bullet collided with TargetBox" << std::endl;
	}
}
