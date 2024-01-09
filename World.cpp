#include "World.h"
#include "Camera.h"
#include "Skybox.h"

// Lights
#include "PointLight.h"
#include "DirectionalLight.h"

// GameObjects
#include "GameObject.h"
#include "Player.h"
#include "TargetBox.h"
#include "Collectible.h"

// Libraries
#include "MeshLibrary.h"
#include "MaterialLibrary.h"

// Input
#include "InputHandler.h"

std::vector<GameObject*> World::gameObjects;
std::vector<GameObject*> World::gameObjectsToAdd;
std::vector<GameObject*> World::gameObjectsToRemove;

Camera* World::camera;

PointLight* World::pointLight;
Light* World::ambientLight;
DirectionalLight* World::directionalLight;

Skybox* World::skybox;


void World::Initialize()
{
	//Skybox
	skybox = new Skybox(MeshLibrary::GetMesh("cube"), MaterialLibrary::GetMaterial("skyday"));
	Renderer::GetInstance().SetSkybox(skybox);

	//Lights
	ambientLight = new Light({ 0.1f,0.1f,0.1f,1.0f });
	ambientLight->SetColor({ 0.45f,0.45f,0.45f,1.0f });

	directionalLight = new DirectionalLight({ 0.0f,0.0f,0.0f,0.0f }, { 0.3f,0.6f,0.3f });
	//directionalLight->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	pointLight = new PointLight({ 1.0f,1.0f,1.0f,1 }, { 0,0,0 }, 20, true);

	//Camera
	camera = new Camera();

	//Action


	//Floor
	GameObject* floor = new GameObject(MeshLibrary::GetMesh("plane"), MaterialLibrary::GetMaterial("checkered"));
	floor->GetMaterial()->texture->tiling = { 150.0f, 150.0f };
	floor->SetPosition({ 0.0f,-2.0f,0.0f });
	gameObjects.push_back(floor);

	//TargetBoxes
	//Fill temp vector with target boxes then push them too gameObjects
	for (size_t i = 0; i < 30; i++)
	{
		TargetBox* targetBox = new TargetBox(MeshLibrary::GetMesh("cube"), MaterialLibrary::GetMaterial("brown"));
		gameObjects.push_back(targetBox);
	}


	for (size_t i = 0; i < 5; i++)
	{
		Collectible* screamingMan = new Collectible(MeshLibrary::GetMesh("scream"), MaterialLibrary::GetMaterial("reflective"));
		gameObjects.push_back(screamingMan);
	}

	//Player
	Player* player = new Player(MeshLibrary::GetMesh("gun"), MaterialLibrary::GetMaterial("gun"), camera);
	gameObjects.push_back(player);

};

void World::SpawnGameObject(GameObject* gameObject)
{
	gameObjectsToAdd.push_back(gameObject);
}

void World::RemoveGameObject(GameObject* gameObject)
{
	gameObjectsToRemove.push_back(gameObject);
}

void World::Update(float deltaTime)
{
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->IsMarkedForRemoval())
		{
			gameObjectsToRemove.push_back(gameObject);
			continue;
		}

		gameObject->Update(deltaTime);
	}


	for (auto& gameObject : gameObjectsToAdd)
	{
		gameObjects.push_back(gameObject);
	}

	gameObjectsToAdd.clear();


	for (auto& gameObject : gameObjectsToRemove)
	{
		gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());
		delete gameObject;
	}

	gameObjectsToRemove.clear();

	LightControls();

}

void World::LightControls()
{
	pointLight->SetPosition(DirectX::SimpleMath::Vector4(camera->GetPosition()));


	// Toggle Point Light
	if (InputHandler::GetInstance().GetKeyDown().P)
	{
		pointLight->SetEnabled(!pointLight->IsEnabled());

		// Change color of text to indicate if the light is on or off
		/*if (pointLight->IsEnabled())
			texts[0]->SetText(L"Point Light: On", DirectX::SimpleMath::Vector4(DirectX::Colors::Green));
		else
			texts[0]->SetText(L"Point Light: Off", DirectX::SimpleMath::Vector4(DirectX::Colors::Red));*/
	}

	// Toggle Directional Light by changing the color from black to white
	if (InputHandler::GetInstance().GetKeyDown().L)
	{
		if (directionalLight->GetColor().x == 0.0f)
		{
			directionalLight->SetColor({ 1.0f,1.0f,1.0f,1.0f });

			//texts[4]->SetText(L"Directional Light: On", DirectX::SimpleMath::Vector4(DirectX::Colors::Green));
		}

		else
		{
			directionalLight->SetColor({ 0.0f,0.0f,0.0f,1.0f });

			//texts[4]->SetText(L"Directional Light: Off", DirectX::SimpleMath::Vector4(DirectX::Colors::Red));
		}

	}

	// Toggle Ambient Light by changing the color from black to white
	if (InputHandler::GetInstance().GetKeyDown().M)
	{
		if (ambientLight->GetColor().x == 0.0f)
		{
			Renderer::GetInstance().GetSkybox()->SetMaterial(MaterialLibrary::GetMaterial("skyday"));
			ambientLight->SetColor({ 0.45f,0.45f,0.45f,1.0f });

			//texts[3]->SetText(L"Ambient Light: On", DirectX::SimpleMath::Vector4(DirectX::Colors::Green));

		}

		else
		{
			Renderer::GetInstance().GetSkybox()->SetMaterial(MaterialLibrary::GetMaterial("skynight"));
			ambientLight->SetColor({ 0.0f,0.0f,0.0f,1.0f });

			//texts[3]->SetText(L"Ambient Light: Off", DirectX::SimpleMath::Vector4(DirectX::Colors::Red));
		}

	}

	if (InputHandler::GetInstance().ScrolledUp())
	{
		pointLight->SetStrength(pointLight->GetStrength() + 5.0f);

		float strength = pointLight->GetStrength();

		// Format the float value to display with limited decimal places
		std::wstring labelText = L"Point Light Intensity: " + std::to_wstring(static_cast<long double>(strength));
		size_t decimalPos = labelText.find(L'.');
		if (decimalPos != std::wstring::npos) {
			size_t decimalPlaces = labelText.size() - decimalPos - 1;
			if (decimalPlaces > 2) {
				labelText = labelText.substr(0, decimalPos + 3);
			}
		}

		// Set the text using the combined wide string
		//texts[1]->SetText(labelText);

	}

	if (InputHandler::GetInstance().ScrolledDown())
	{
		pointLight->SetStrength(pointLight->GetStrength() - 5.0f);

		float strength = pointLight->GetStrength();

		// Format the float value to display with limited decimal places
		std::wstring labelText = L"Point Light Intensity: " + std::to_wstring(static_cast<long double>(strength));
		size_t decimalPos = labelText.find(L'.');
		if (decimalPos != std::wstring::npos) {
			size_t decimalPlaces = labelText.size() - decimalPos - 1;
			if (decimalPlaces > 2) {
				labelText = labelText.substr(0, decimalPos + 3);
			}
		}

		// Set the text using the combined wide string
		//texts[1]->SetText(labelText);
	}

	if (InputHandler::GetInstance().GetKeyDown().B)
	{
		// Change point light to a random color
		float r = static_cast<float>(rand()) / RAND_MAX; // Generate random values for RGB components
		float g = static_cast<float>(rand()) / RAND_MAX;
		float b = static_cast<float>(rand()) / RAND_MAX;

		DirectX::SimpleMath::Vector4 color = { r, g, b, 1 };

		pointLight->SetColor(color);



		//texts[2]->SetText(L"Point Light Color: Random", color);
	}
}


