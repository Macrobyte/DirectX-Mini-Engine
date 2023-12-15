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

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* zBuffer;
	ID3D11BlendState* alphaBlend;

	ID3D11RasterizerState* rasterSolid;
	ID3D11RasterizerState* rasterSkybox;
	ID3D11DepthStencilState* depthWriteSolid;
	ID3D11DepthStencilState* depthWriteSkybox;
	ID3D11RasterizerState* rasterState;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;

	HRESULT InitializeDirectX(HWND hWnd);
	void InitializeViewport();
	HRESULT InitializePipeline();

	HRESULT LoadVertexShader(LPCWSTR filename, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout);
	HRESULT LoadPixelShader(LPCWSTR filename, ID3D11PixelShader** pixelShader);

public:
	HRESULT Initialize(HWND hWnd);
	void Release();

	void Render();

#pragma region Getters
	ID3D11Device* GetDevice() { return device; }
	ID3D11DeviceContext* GetDeviceContext() { return deviceContext; }
#pragma endregion
};

