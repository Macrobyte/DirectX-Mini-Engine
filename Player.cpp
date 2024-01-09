#define NOMINMAX
#include "Player.h"
#include "Camera.h"
#include "InputHandler.h"
#include "BoundingBox.h"
#include <iostream>
#include <algorithm>
#include "MeshLibrary.h"
#include "MaterialLibrary.h"
#include "World.h"
#include "Bullet.h"
#include "SoundHandler.h"



Player::Player(Mesh* mesh, Material* material, Camera* camera) 
	: GameObject(mesh, material)
{
	this->camera = camera;
	SetTag(T_Player);
	SetName("Player");

	SetPosition({ 10.0f, 0.0f, 0.0f });

	SetScale({ 0.1f,0.1f,0.1f });

	GetBoundingBox()->SetScale({ 0.1f,0.1f,0.1f });



}

Player::~Player()
{
	GameObject::~GameObject();

	delete camera;
}

void Player::BeginPlay()
{
	GameObject::BeginPlay();
}

void Player::Update(float deltaTime)
{
	GameObject::Update(deltaTime);

	HandleMovement(deltaTime);
	HandleRotation(deltaTime);

	if(InputHandler::GetInstance().GetMouseButtonDown(B_LeftButton))
		Shoot();

}

void Player::OnCollision(BoundingBox* other)
{
	GameObject::OnCollision(other);

}

void Player::HandleMovement(float deltaTime)
{
	Vector3 horizontalForward = GetTransform().GetForwardVector();
	horizontalForward.y = 0.0f; // Set the vertical component to zero

	if (InputHandler::GetInstance().GetKey().W)
		SetPosition(GetPosition() + horizontalForward * speed * deltaTime);

	if (InputHandler::GetInstance().GetKey().S)
		SetPosition(GetPosition() - horizontalForward * speed * deltaTime);

	Vector3 horizontalRight = GetTransform().GetRightVector();
	horizontalRight.y = 0.0f; // Set the vertical component to zero

	if (InputHandler::GetInstance().GetKey().A)
		SetPosition(GetPosition() + horizontalRight * speed * deltaTime);

	if (InputHandler::GetInstance().GetKey().D)
		SetPosition(GetPosition() - horizontalRight * speed * deltaTime);
		
}

void Player::HandleRotation(float deltaTime)
{
    float xInput = InputHandler::GetInstance().GetMouseXY().x * sensitivity;
    //float yInput = InputHandler::GetInstance().GetMouseXY().y * sensitivity;

    Quaternion xQuaternion = Quaternion::CreateFromAxisAngle(Vector3::Up, xInput);

	SetRotation(GetRotation() * xQuaternion);

#pragma region Stupid Quaternions
 //   Quaternion yQuaternion = Quaternion::CreateFromAxisAngle(GetTransform().GetRightVector(), yInput);

	//Quaternion temp = GetRotation() * yQuaternion;

	////
	//
	//
	//// Calculate current pitch angle and ensure it's within a valid range
	//float currentPitch = std::asin(2 * (temp.x * temp.z - temp.w * temp.y));
	//if (!std::isnan(currentPitch)) {
	//	float maxPitch = DirectX::XM_PI / 3.0f;  // Maximum allowed pitch
	//	float minPitch = -DirectX::XM_PI / 2.0f; // Minimum allowed pitch

	//	float targetPitch = currentPitch + yInput;
	//	targetPitch = std::min(std::max(targetPitch, minPitch), maxPitch);
	//	float pitchDiff = targetPitch - currentPitch;

	//	// Apply the pitch rotation within the valid range
	//	Quaternion pitchRotation = Quaternion::CreateFromAxisAngle(GetTransform().GetRightVector(), -pitchDiff);

	//	// Calculate the resulting rotation without applying it yet
	//	Quaternion resultingRotation = GetRotation() * xQuaternion * pitchRotation;

	//	// Calculate the yaw rotation
	//	Quaternion yawRotation = Quaternion::CreateFromAxisAngle(Vector3::Up, xInput);

	//	// Check if the angle between the current and resulting rotation is within the limit
	//	if (Quaternion::Angle(temp * pitchRotation, Quaternion::Identity) < maxPitch) {
	//		
	//		SetRotation(resultingRotation);
	//	}
	//	else
	//	{
	//		std::cout << "Rotation limit reached\n";
	//	}
	//}

#pragma endregion

	camera->SetPosition(GetPosition() + GetTransform().GetForwardVector() * -0.53f + GetTransform().GetUpVector() * 0.20f + GetTransform().GetRightVector() * 0.37f);
	camera->rotation = GetRotation();
	GetRotation().Normalize();
}

void Player::Shoot()
{
	// Create a bullet
	Bullet* test = new Bullet(MeshLibrary::GetMesh("cube"), MaterialLibrary::GetMaterial("reflective"), 22.0f, 2.0f);

	test->SetPosition(GetPosition() + camera->ForwardVector() * 2.5f);
	test->SetRotation(GetRotation());
	Vector3 scale = { 0.1f,0.1f,0.1f };
	test->SetScale(scale);
	test->GetBoundingBox()->SetScale(scale);

	SoundHandler::GetInstance().PlayOneShot("Shoot");

	// Add the bullet to the scene
	World::SpawnGameObject(test);
}
