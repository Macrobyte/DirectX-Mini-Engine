#include "Renderer.h"

#include <DirectXColors.h>
#include <iostream>

#include "Engine/Core/Window.h"

HRESULT Renderer::InitializeDirectX(HWND hWnd)
{
#pragma region Create Swap Chain Description
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//32-bit color
	swapChainDesc.BufferDesc.Width = Window::GetWindowWidth();
	swapChainDesc.BufferDesc.Height = Window::GetWindowHeight();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;			// 60 FPS
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;			// 60/1 = 60 FPS
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// How swap chain is to be used
	swapChainDesc.OutputWindow = hWnd;								// The window to be used
	swapChainDesc.SampleDesc.Count = 1;								// How many multi samples
	swapChainDesc.Windowed = TRUE;									// Windowed or full screen
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// Allow full screen switching

#pragma endregion

#pragma region Create Device and Swap Chain
	HRESULT hr;
		
	hr = D3D11CreateDeviceAndSwapChain(nullptr, // Which graphics adapter to use, NULL = default
		D3D_DRIVER_TYPE_HARDWARE,				// Driver type to use
		nullptr,								// Software driver handle, used when D3D_DRIVER_TYPE_SOFTWARE is used
		D3D11_CREATE_DEVICE_DEBUG,				// Flags for runtime layers
		nullptr,								// Feature levels array
		NULL,									// Number of feature levels in array
		D3D11_SDK_VERSION,						// SDK version
		&swapChainDesc,							// Swap chain description
		&swapChain,								// Swap chain address
		&device,								// Device address
		nullptr,								// Supported feature level
		&deviceContext);						// Device context address

	if(FAILED(hr))
		return hr;

	ID3D11Texture2D* backBufferTexture = nullptr;

	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture)); // Get back buffer from swap chain

	if (FAILED(hr))
		return hr;

	hr = device->CreateRenderTargetView(backBufferTexture, nullptr, &renderTargetView); // Create render target view from back buffer

	if (FAILED(hr))
		return hr;

	backBufferTexture->Release(); // Release back buffer texture as it is no longer needed

	deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr); // Set the back buffer as the current render target

#pragma endregion

	InitializeViewport();

	std::cout << "DirectX initialized" << std::endl;

	return S_OK;
}

void Renderer::InitializeViewport()
{
	D3D11_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = static_cast<float>(Window::GetWindowWidth());
	viewPort.Height = static_cast<float>(Window::GetWindowHeight());
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	deviceContext->RSSetViewports(1, &viewPort);

	std::cout << "Viewport initialized" << std::endl;
}

HRESULT Renderer::Initialize(HWND hWnd)
{
	if(FAILED(InitializeDirectX(hWnd)))
	{
		MessageBeep(MB_ICONSTOP);
		MessageBox(nullptr, L"Failed to initialize DirectX", L"Critical Error!", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

/**
 * \brief Release all DirectX resources
 */
void Renderer::Release()
{
	if(renderTargetView) renderTargetView->Release();
	if(swapChain) swapChain->Release();
	if(device) device->Release();
	if(deviceContext) deviceContext->Release();
}


/**
 * \brief Renders a frame
 */
void Renderer::Render()
{
	deviceContext->ClearRenderTargetView(renderTargetView, DirectX::Colors::CornflowerBlue);


	swapChain->Present(0, 0);
}
