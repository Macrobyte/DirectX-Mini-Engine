#pragma once
#include "../../Utility/Singleton.h"
#include "../../Rendering/Renderer.h"
#include "../UI/UI.h"


class Game : public Singleton<Game>
{
	friend class Singleton<Game>;

private:
	Game() {};
	~Game() {};
	UI* ui;

	int score = 0;

	float fpsTimer = 0.0f;
	int fps = 0;

public:
		void Initialize(HINSTANCE hInstance, int nCmdShow);
		void InitializeGame();

		void InitializeUI();

		void AddScore(int score);
		void DrawFPS();

		void BeginPlay();
		void HandleInput();
		void Update();
		void Render();
		void Shutdown();

		UI* GetUI() { return ui; }


	

};

