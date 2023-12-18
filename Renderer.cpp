#include "Renderer.h"

#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <iostream>

#include "Engine/Core/Window.h"
#include "ReadData.h"

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

	if (FAILED(hr))
		return hr;

	ID3D11Texture2D* backBufferTexture = nullptr;

	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture)); // Get back buffer from swap chain

	if (FAILED(hr))
		return hr;

	hr = device->CreateRenderTargetView(backBufferTexture, nullptr, &renderTargetView); // Create render target view from back buffer

	if (FAILED(hr))
		return hr;

	backBufferTexture->Release(); // Release back buffer texture as it is no longer needed
#pragma endregion

#pragma region Depth Stencil
	D3D11_TEXTURE2D_DESC texture2DDesc = { 0 };
	texture2DDesc.Width = Window::GetWindowWidth();
	texture2DDesc.Height = Window::GetWindowHeight();
	texture2DDesc.ArraySize = 1;
	texture2DDesc.MipLevels = 1;
	texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2DDesc.SampleDesc.Count = swapChainDesc.SampleDesc.Count; // Same sample description as the swap chain
	texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2DDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* zBufferTexture;
	if (FAILED(device->CreateTexture2D(&texture2DDesc, NULL, &zBufferTexture)))
	{
		OutputDebugString(L"Failed to create depth stencil texture\n");
		return E_FAIL;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilDesc.Format = texture2DDesc.Format;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	if (FAILED(device->CreateDepthStencilView(zBufferTexture, &depthStencilDesc, &zBuffer)))
	{
		OutputDebugString(L"Failed to create depth stencil view\n");
		return E_FAIL;
	}

	zBufferTexture->Release();
#pragma endregion

	deviceContext->OMSetRenderTargets(1, &renderTargetView, zBuffer); // Set the back buffer as the current render target

	InitializeViewport();

#pragma region Blend State
	D3D11_BLEND_DESC blendState = { 0 };
	blendState.RenderTarget[0].BlendEnable = FALSE;
	blendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendState.IndependentBlendEnable = FALSE;
	blendState.AlphaToCoverageEnable = FALSE;

	device->CreateBlendState(&blendState, &alphaBlend);

#pragma endregion

#pragma region Culling
	// Backface culling
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FillMode = D3D11_FILL_SOLID;

	device->CreateRasterizerState(&rsDesc, &rasterSolid);

	// Frontface culling
	rsDesc.CullMode = D3D11_CULL_FRONT;
	device->CreateRasterizerState(&rsDesc, &rasterSkybox);

	// Depth writing enabled
	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &depthWriteSolid);

	// Depth writing disabled
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	device->CreateDepthStencilState(&dsDesc, &depthWriteSkybox);

	deviceContext->RSSetState(rasterSolid);
#pragma endregion

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

HRESULT Renderer::InitializePipeline()
{
	if (FAILED(LoadVertexShader(L"Compiled Shaders/VertexShader.cso", &vertexShader, &inputLayout)))
	{
		OutputDebugString(L"Failed to load vertex shader\n");
		return E_FAIL;
	}

	if (FAILED(LoadPixelShader(L"Compiled Shaders/PixelShader.cso", &pixelShader)))
	{
		OutputDebugString(L"Failed to load pixel shader\n");
		return E_FAIL;
	}

	if (FAILED(LoadVertexShader(L"Compiled Shaders/SkyboxVShader.cso", &vertexShader, &inputLayout)))
	{
		OutputDebugString(L"Failed to load vertex shader\n");
		return E_FAIL;
	}

	if (FAILED(LoadPixelShader(L"Compiled Shaders/SkyboxPShader.cso", &pixelShader)))
	{
		OutputDebugString(L"Failed to load pixel shader\n");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Renderer::LoadVertexShader(LPCWSTR filename, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout)
{
	auto vertexShaderBytecode = DX::ReadData(filename);

	if (FAILED(device->CreateVertexShader(vertexShaderBytecode.data(), vertexShaderBytecode.size(), NULL, vertexShader)))
	{
		OutputDebugString(L"Failed to create vertex shader\n");
		return E_FAIL;
	}

	deviceContext->VSSetShader(this->vertexShader, 0, 0);

	ID3D11ShaderReflection* vertexShaderReflection = NULL;

	if (FAILED(D3DReflect(vertexShaderBytecode.data(), vertexShaderBytecode.size(), IID_ID3D11ShaderReflection, (void**)&vertexShaderReflection)))
	{
		OutputDebugString(L"Failed to reflect vertex shader\n");
		return E_FAIL;
	}

	D3D11_SHADER_DESC shaderDesc;
	vertexShaderReflection->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	D3D11_SIGNATURE_PARAMETER_DESC* signatureParamDesc = new D3D11_SIGNATURE_PARAMETER_DESC[shaderDesc.InputParameters]{ 0 }; 
	for (UINT i = 0; i < shaderDesc.InputParameters; i++)
	{
		vertexShaderReflection->GetInputParameterDesc(i, &signatureParamDesc[i]);
	}

	// Signature param desk masks as follows: float4 = 15, float3 = 7, float2 = 3, float[1] = 1. These are bitmasks
	D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[shaderDesc.InputParameters]{ 0 };
	for (size_t i = 0; i < shaderDesc.InputParameters; i++)
	{
		ied[i].SemanticName = signatureParamDesc[i].SemanticName;
		ied[i].SemanticIndex = signatureParamDesc[i].SemanticIndex;
		if (signatureParamDesc[i].ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
		{
			switch (signatureParamDesc[i].Mask)
			{
			case 1:  ied[i].Format = DXGI_FORMAT_R32_FLOAT;			 break;	// Float 1
			case 3:  ied[i].Format = DXGI_FORMAT_R32G32_FLOAT;		 break;	// Float 2
			case 7:	 ied[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;	 break;	// Float 3
			case 15: ied[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break; // Float 4
			default: break;
			}
		}// The above only covers ---x, --yx, -zyx, wzyx. It may be possible for a mask to be -yx- or yx- or zyx- (6, 12, 14)
		ied[i].InputSlot = 0;
		ied[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		ied[i].InstanceDataStepRate = 0;
	}


	if (FAILED(device->CreateInputLayout(ied, shaderDesc.InputParameters, vertexShaderBytecode.data(), vertexShaderBytecode.size(), inputLayout)))
	{
		OutputDebugString(L"Failed to create input layout\n");
		return E_FAIL;
	}

	deviceContext->IASetInputLayout(this->inputLayout);

	delete[] signatureParamDesc;
	delete[] ied;

	return S_OK;
}

HRESULT Renderer::LoadPixelShader(LPCWSTR filename, ID3D11PixelShader** pixelShader)
{
	auto pixelShaderBytecode = DX::ReadData(filename);

	if (FAILED(device->CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), NULL, pixelShader)))
	{
		OutputDebugString(L"Failed to create pixel shader\n");
		return S_OK;
	}
}

HRESULT Renderer::Initialize(HWND hWnd)
{
	if(FAILED(InitializeDirectX(hWnd)))
	{
		MessageBeep(MB_ICONSTOP);
		MessageBox(nullptr, L"Failed to initialize DirectX", L"Critical Error!", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	if (FAILED(InitializePipeline()))
	{
		MessageBeep(MB_ICONSTOP);
		MessageBox(nullptr, L"Failed to initialize Pipeline", L"Critical Error!", MB_ICONERROR | MB_OK);
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

	deviceContext->ClearDepthStencilView(zBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Objects to render go here


	deviceContext->OMSetBlendState(alphaBlend, nullptr, 0xffffffff);

	swapChain->Present(0, 0);
}
