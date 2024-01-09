#pragma once
#include <vector>
#include "DirectionalLight.h"

class GameObject;
class Camera;
class PointLight;
class Light;
class DirectionalLight;
class Text;
class Skybox;

class World
{
private:
	static Skybox* skybox;

	static std::vector<GameObject*> gameObjects;
	static std::vector<GameObject*> gameObjectsToAdd;
	static std::vector<GameObject*> gameObjectsToRemove;

	static Camera* camera;

	static Light* ambientLight;
	static PointLight* pointLight;
	static DirectionalLight* directionalLight;



public:
	static void Initialize();

	static void SpawnGameObject(GameObject* gameObject);
	static void RemoveGameObject(GameObject* gameObject);

	static void Update(float deltaTime);

	static void LightControls();

	static std::vector<GameObject*> GetGameObjects() { return gameObjects; }
	static Camera* GetCamera() { return camera; }
	static void SetCamera(Camera* cam) { camera = cam; }

	static Light* GetAmbientLight() { return ambientLight; }
	static DirectionalLight* GetDirectionalLight() { return directionalLight; }
	static PointLight* GetPointLight() { return pointLight; }

	static Skybox* GetSkybox() { return skybox; }

};

