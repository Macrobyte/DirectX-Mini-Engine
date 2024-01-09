#include "Game.h"
#include "Engine/Core/Window.h"
#include "GameObject.h"
#include "Engine\Core\Time.h"
#include "InputHandler.h"

#include "MeshLibrary.h"
#include "MaterialLibrary.h"

#include "Text.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "CollisionHandler.h"

#include "World.h"

#include "SoundHandler.h"

void Game::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	if (FAILED(Window::Initialize(Window::WindowProc, hInstance, nCmdShow, L"Game", 1280, 720)))
	{
		MessageBeep(MB_ICONSTOP);
		MessageBox(NULL, L"Failed to create window", L"Critical Error!", MB_ICONERROR | MB_OK);
	}

	if (FAILED(Renderer::GetInstance().Initialize(Window::GetWindowHandle())))
	{
		MessageBeep(MB_ICONSTOP);
		MessageBox(NULL, L"Failed to create renderer", L"Critical Error!", MB_ICONERROR | MB_OK);
	}

	MeshLibrary::LoadAllMeshes(); // Also loads Objs

	MaterialLibrary::LoadAllMaterials();

	Time::Initialize();

	InputHandler::GetInstance().Initialize();
	
	SoundHandler::GetInstance().Initialize();

	World::Initialize();

	InitializeGame();

	BeginPlay();


}

void Game::InitializeGame()
{
	SoundHandler::GetInstance().PlayMusic("MainTheme");
	//SoundHandler::GetInstance().PlayOneShot("Shoot");

	InitializeUI();

}

void Game::InitializeUI()
{
	ui = new UI();

	std::vector<Text*> textElements;

	// Instructions Panel
	{
		Text* info1 = new Text({ 0,0 }, L"Press 'F1' to toggle instructions!");

		textElements.push_back(info1);

		UIPanel* instructionsPanel = new UIPanel("Instructions Panel", textElements);
		instructionsPanel->SetPosition({ 0,700 });

		ui->AddPanel(instructionsPanel);

		textElements.clear();
	}


	// Light instructions panel
	{
		Text* lightInfo1 = new Text({ 0,0 }, L"Press 'P' to turn Point Light On/Off.");
		Text* lightInfo2 = new Text({ 0,20 }, L"Scroll Up/Down to Increase/Decrease Point Light intensity.");
		Text* lightInfo3 = new Text({ 0,40 }, L"Press 'B' to change Point Light color.");
		Text* lightInfo4 = new Text({ 0,60 }, L"Press 'L' to turn Directional Light On/Off.");
		Text* lightInfo5 = new Text({ 0,80 }, L"Press 'M' to turn Ambient Light On/Off.");

		textElements.push_back(lightInfo1);
		textElements.push_back(lightInfo2);
		textElements.push_back(lightInfo3);
		textElements.push_back(lightInfo4);
		textElements.push_back(lightInfo5);


		UIPanel* LightsPanel = new UIPanel("Lights Panel", textElements);


		ui->AddPanel(LightsPanel);

		textElements.clear();
	}

	// Score Panel
	{
		Text* scoreText = new Text({ 0,0 }, L"Score: ");
		Text* score = new Text({ 90,0 }, L"0", "ScoreText");

		textElements.push_back(scoreText);
		textElements.push_back(score);

		UIPanel* scorePanel = new UIPanel("Score Panel", textElements);

		scorePanel->SetPosition({ 0,680 });

		ui->AddPanel(scorePanel);
		textElements.clear();
	}

	// FPS Panel
	{
		Text* fpsText = new Text({ 0,0 }, L"FPS: ");
		Text* fps = new Text({ 50,0 }, L"0", "FPSText");
		textElements.push_back(fpsText);
		textElements.push_back(fps);
		UIPanel* fpsPanel = new UIPanel("FPS Panel", textElements);
		fpsPanel->SetPosition({ 0,660 });
		ui->AddPanel(fpsPanel);
		textElements.clear();
	}
	
}

void Game::AddScore(int score)
{
	this->score += score; 
	GetUI()->GetPanel("Score Panel")->SetTextElement(std::to_wstring(this->score), "ScoreText");
}

void Game::DrawFPS()
{
	fpsTimer += Time::DeltaTime();
	++fps;

	if (fpsTimer >= 1.0f)
	{
		GetUI()->GetPanel("FPS Panel")->SetTextElement(std::to_wstring(fps), "FPSText");
		fps = 0;
		fpsTimer = 0.0f;
	}
	
}


void Game::BeginPlay()
{
	for (auto& gameObject : World::GetGameObjects())
		gameObject->BeginPlay();
}

void Game::HandleInput()
{
	InputHandler::GetInstance().HandleInput();

	if (InputHandler::GetInstance().GetKeyDown().F1)
		ui->GetPanel("Lights Panel")->SetActive(!ui->GetPanel("Lights Panel")->IsActive());

	if (InputHandler::GetInstance().GetKeyDown().G)
		ui->GetPanel("Lights Panel")->SetPosition({ ui->GetPanel("Lights Panel")->GetPosition().x + 1.0f, ui->GetPanel("Lights Panel")->GetPosition().y });

	if (InputHandler::GetInstance().GetKeyDown().Escape)
		PostQuitMessage(0);
}

void Game::Update()
{
	Time::Update();

	World::Update(Time::DeltaTime());
	
	DrawFPS();

	CollisionHandler::Update();

	SoundHandler::GetInstance().Update();
}

void Game::Render()
{
	Renderer::GetInstance().Render();
}

void Game::Shutdown()
{
	Renderer::GetInstance().Release();
	MaterialLibrary::ReleaseAllMaterials();
}
