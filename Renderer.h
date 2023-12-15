#pragma once
#include <d3d11.h>

#include "Singleton.h"

class Window;

class Renderer : public Singleton<Renderer>
{
	friend class Singleton<Renderer>; // Singleton class can access private constructor

public:
	Renderer(){};
	~Renderer(){};

private:

	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3D11RenderTargetView* renderTargetView;

	HRESULT InitializeDirectX(HWND hWnd);
	void InitializeViewport();

public:
	HRESULT Initialize(HWND hWnd);
	void Release();

	void Render();

#pragma region Getters
	ID3D11Device* GetDevice() { return device; }
	ID3D11DeviceContext* GetDeviceContext() { return deviceContext; }
	IDXGISwapChain* GetSwapChain() { return swapChain; }
#pragma endregion
};

