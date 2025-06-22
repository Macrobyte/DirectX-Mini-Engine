#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Utility/Singleton.h"
#include <vector>
#include <string>
#include <SpriteFont.h>
#include "../Utility/TypeHelpers.h"

#define MAX_POINT_LIGHTS 4



class Window;
class GameObject;
class Skybox;

class Renderer : public Singleton<Renderer>
{
	friend class Singleton<Renderer>; // Singleton class can access private constructor

public:
	Renderer(){};
	~Renderer(){};

private:

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;

	IDXGISwapChain* swapChain = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11DepthStencilView* zBuffer = nullptr;
	ID3D11BlendState* alphaBlend = nullptr;

	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	bool isFullscreen = false;

	Skybox* skybox = nullptr;

	// Text rendering
	std::unique_ptr<DirectX::SpriteFont> font;
	std::vector<std::unique_ptr<DirectX::SpriteFont>> fonts;
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;

	void InitializeTextRendering();
	void LoadFont(std::wstring filePath);
	void RenderUI();


	HRESULT InitializeDirectX(HWND hWnd);
	void InitializeViewport();

public:
	HRESULT Initialize(HWND hWnd);
	void Render();
	void Release();


	HRESULT LoadVertexShader(LPCWSTR filename, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout);
	HRESULT LoadPixelShader(LPCWSTR filename, ID3D11PixelShader** pixelShader);
#pragma region Getters
	ID3D11Device* GetDevice() { return device; }
	ID3D11DeviceContext* GetDeviceContext() { return deviceContext; }
	Skybox* GetSkybox() { return skybox; }
#pragma endregion

#pragma region Setters
	void SetSkybox(Skybox* skybox) { this->skybox = skybox; }
#pragma endregion

	void ToggleFullscreen();
};

