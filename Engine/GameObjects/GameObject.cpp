#include "GameObject.h"
#include "../Core/InputHandler.h"
#include <iostream>
#include "../GameObjects/Components/BoundingBox.h"

GameObject::GameObject(Mesh* mesh, Material* material)
{
	transform = Transform();
	this->mesh = mesh;
	this->material = material;
	name = "Default";
	tag = T_Default;

	boundingBox = new BoundingBox();
	boundingBox->SetPosition(transform.GetPosition());
	boundingBox->SetRotation(transform.GetRotation());
	boundingBox->SetScale(transform.GetScale());
	boundingBox->SetGameObject(this);
	boundingBox->SetOnCollideCallback(std::bind(&GameObject::OnCollision, this, std::placeholders::_1));
}

GameObject::~GameObject()
{
	delete boundingBox;
}

void GameObject::BeginPlay()
{
	std::cout << GetName() << " BeginPlay" << std::endl;
}

void GameObject::Update(float deltaTime)
{
	boundingBox->SetPosition(GetTransform().GetPosition());
	boundingBox->SetRotation(GetTransform().GetRotation());


	if (isAnimated)
	{
		Animate(deltaTime);
	}
	
}

void GameObject::OnCollision(BoundingBox* other)
{
	//std::cout << boundingBox->GetGameObject()->GetName() << " collided with " << other->GetGameObject()->GetName() << std::endl;

	if (other->GetGameObject()->GetTag() == T_TargetBox)
	{
		collided = true;


	}

	if (other->GetGameObject()->GetTag() == T_Collectible)
	{
		collided = true;
	}
}

void GameObject::Animate(float deltaTime)
{
	static float timeElapsed = 0.0f; // Initialize elapsed time

	// Bobbing up and down along the Y-axis (vertically)
	float bobMagnitude = 0.1f; // Adjust this value to change the bobbing intensity
	float bobSpeed = 0.5f;     // Adjust this value to change the bobbing speed
	timeElapsed += deltaTime;  // Accumulate elapsed time

	// Calculate the offset (bobbing motion within a limited range)
	float bobPeriod = 2.0f * DirectX::XM_PI / bobSpeed; // Time for one full bobbing cycle
	float bobOffset = bobMagnitude * sinf(timeElapsed * bobSpeed); // Calculate the offset

	// Set the position for bobbing
	Vector3 currentPosition = GetPosition();
	currentPosition.y = bobOffset; // Update the Y-position for bobbing
	SetPosition(currentPosition);

	// Spin around its Y-axis (rotation)
	float rotateSpeed = 1.0f; // Adjust this value to change the rotation speed
	Quaternion currentRotation = GetRotation();
	currentRotation *= Quaternion::CreateFromYawPitchRoll(rotateSpeed * deltaTime, 0.0f, 0.0f); // Adjust the Y-rotation
	SetRotation(currentRotation);
}

void GameObject::GenerateRandomPosition(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
	const int maxAttempts = 10; // Maximum attempts to find a non-colliding position

	for (int attempt = 0; attempt < maxAttempts; ++attempt) {
		float diffX = maxX - minX;
		float diffY = maxY - minY;
		float diffZ = maxZ - minZ;

		if (diffX != 0 && diffY != 0 && diffZ != 0) {
			Vector3 newPosition = {
				static_cast<float>(rand()) / (RAND_MAX / diffX) + minX,
				static_cast<float>(rand()) / (RAND_MAX / diffY) + minY,
				static_cast<float>(rand()) / (RAND_MAX / diffZ) + minZ
			};

			// Your collision detection logic
			if (!collided) {
				// Set the position if there's no collision
				SetPosition(newPosition);
				collided = false;
				return; // Exit the function after setting the position
			}
		}
		else {
			// Handle the case where division by zero might occur
			// (e.g., logging a message, using a default position, or other handling)
			std::cout << "Division by zero might occur!" << std::endl;
			return; // Exit the function to prevent potential division by zero
		}
	}

	// Handle the case where a suitable position couldn't be found after maximum attempts
	// (e.g., logging a message, using a default position, or other handling)
	std::cout << "Failed to find a non-colliding position within max attempts." << std::endl;

}

