#pragma comment(lib, "dxguid.lib")

#include <iostream>
#include <random>

#include <d3d11.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include "ReadData.h"
#include "text2D.h"

#include "GameWindow.h"

#include "WICTextureLoader.h"

using namespace DirectX;

#pragma region Global Variables

XMVECTORF32 clearColor = Colors::Aquamarine; // Default clear color

// Direct3D Variables
IDXGISwapChain* g_pSwapChain = NULL; // The pointer to the swap chain interface
ID3D11Device* g_pDevice = NULL; // The pointer to our Direct3D device interface
ID3D11DeviceContext* g_pDeviceContext = NULL; // The pointer to our Direct3D device context
ID3D11RenderTargetView* g_backbuffer = NULL; // A view to access our back buffer

// Shaders
ID3D11VertexShader* g_pVertexShader = NULL; // The pointer to the vertex shader
ID3D11PixelShader* g_pPixelShader = NULL; // The pointer to the pixel shader
ID3D11InputLayout* g_pInputLayout = NULL; // The pointer to the input layout

ID3D11Buffer* g_pVertexBuffer = NULL; // The pointer to the vertex buffer

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT2 UV;
};

ID3D11Buffer* pVBuffer = NULL; // Vertex
ID3D11Buffer* pIBuffer = NULL; // Index
ID3D11Buffer* pCBuffer = NULL; // Constant

struct CBUFFER0
{
	XMMATRIX WVP; // 64 bytes World View Projection matrix
	// The 64 comes from each row being 16 bytes (4 floats * 4 bytes each) and there being 4 rows
	// Each '4' is a 4 byte float value
	// [ 4,4,4,4 ]
	// [ 4,4,4,4 ]
	// [ 4,4,4,4 ]
	// [ 4,4,4,4 ]
	// XMMATRIX is a a strictly aligned type for SIMD hardware.
	
	//float padding; // 4 bytes! - not used!!!!! but important!!!!!!!!!!!!!!!!
};

// Transforms
XMFLOAT3 pos{ 0,0,1 };
XMFLOAT3 rot{ 0,0,0 };
XMFLOAT3 scl{ 1,1,1 };


struct Camera
{
	float x = 0, y = 0, z = 0;
	float pitch = XM_PIDIV2, yaw = 0;

	XMVECTOR ForwardVector()
	{ 
		return XMVectorSet(sin(yaw) * sin(pitch), cos(pitch), cos(yaw) * sin(pitch), 1.f);
	};

	XMVECTOR RightVector() 
	{ 
		return XMVectorSet(sin(yaw - XM_PIDIV2), 0, cos(yaw - XM_PIDIV2), 1.f);
	}
};

Camera g_camera;

ID3D11ShaderResourceView* pTexture = NULL; // Texture
ID3D11SamplerState* pSampler = NULL; // Sampler

Text2D* pText;

ID3D11BlendState* pAlphaBlendEnable = NULL;
ID3D11BlendState* pAlphaBlendDisable = NULL;

ID3D11RasterizerState* pRasterizerState = NULL;

#pragma endregion

#pragma region Function Properties
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

// Direct3D Functions
HRESULT InitD3D(HWND hWnd); // Sets up and initializes Direct3D
void CleanD3D(); // Closes Direct3D and releases memory
void RenderFrame();

void InitGraphics(); // Create the shape to render
HRESULT InitPipeline(); // Loads and prepares the shaders

// Console window for debug output
void OpenConsole();
#pragma endregion


// Entry point for our windows application
int WINAPI WinMain(
	_In_ HINSTANCE hInstance, // Handle to this instance (our app loaded into memory)
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, // Command line arguments
	_In_ int nCmdShow) // How the window is to be shown (e.g. maximised, minimised, etc.
{

	OpenConsole();

	if (FAILED(GameWindow::InitWindow(WindowProc, hInstance, nCmdShow, L"Game", 800, 600)))
	{
		MessageBeep(MB_ICONSTOP);
		MessageBox(NULL, L"Failed to create window", L"Critical Error!", MB_ICONERROR | MB_OK);
	}

	if (FAILED(InitD3D(GameWindow::GetWindowHandle())))
	{
		MessageBeep(MB_ICONSTOP);
		MessageBox(NULL, L"Failed to create Direct3D device and context", L"Critical Error!", MB_ICONERROR | MB_OK);
	}

	if (FAILED(InitPipeline()))
	{
		MessageBeep(MB_ICONSTOP); 
		MessageBox(NULL, L"Failed to create pipeline", L"Critical Error!", MB_ICONERROR | MB_OK); 
	}

	InitGraphics();

	// Used to hold windows event messages
	MSG msg;
	
	// Enter main loop
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate certain messages into correct format, namely key presses
			TranslateMessage(&msg);

			// Send the message to the WindowProc function
			DispatchMessage(&msg);

			//Break out of the loop if a quite message is detected
			if (msg.message == WM_QUIT)
				break;
		}
		else
		{
			// Game Code
			RenderFrame();
		}
	}

	CleanD3D();

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


	switch (message)
	{
		// This message is send when the user closes the window
		// Depending on our handling of your application windows, you may not need this.
		// In this case, if a window is destroyed (e.g when escape is pressed below), we exit the application and post a message
	case WM_DESTROY:
		// Send quit message to the app
		PostQuitMessage(0);
		return 0;
		break;


	case WM_KEYDOWN:

		

		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(GameWindow::GetWindowHandle()); //Note! Destroying the window is not the same as closing the app
			// Destroying the window will poas a WM_DESTROY  which will lead to PostQuitMessage(..) being called
		case VK_LEFT: 
			g_camera.yaw -= XM_PI / 8;
			break;
		case VK_RIGHT: 
			g_camera.yaw += XM_PI / 8;
			break;
		case VK_UP: 
			g_camera.yaw -= XM_PI / 8;
			break;
		case VK_DOWN: 
			g_camera.yaw += XM_PI / 8;
			break;
		case 'W':
			g_camera.x += XMVectorGetX(g_camera.ForwardVector()) * 0.25f;
			g_camera.z += XMVectorGetZ(g_camera.ForwardVector()) * 0.25f;

			break;
		case 'S':
			g_camera.x -= XMVectorGetX(g_camera.ForwardVector()) * 0.25f;
			g_camera.z -= XMVectorGetZ(g_camera.ForwardVector()) * 0.25f;

			break;
		case 'A':
			g_camera.x += XMVectorGetX(g_camera.RightVector()) * 0.25f;
			g_camera.z += XMVectorGetZ(g_camera.RightVector()) * 0.25f;
			break;
		case 'D':
			g_camera.x -= XMVectorGetX(g_camera.RightVector()) * 0.25f;
			g_camera.z -= XMVectorGetZ(g_camera.RightVector()) * 0.25f;
		break;
		
		}
		
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;

}

HRESULT InitPipeline()
{
	// Load and compile the two shaders
	HRESULT result;

	auto vertexShaderBytecode = DX::ReadData(L"Compiled Shaders/VertexShader.cso");
	auto pixelShaderBytecode = DX::ReadData(L"Compiled Shaders/PixelShader.cso");

	// Encapsulate both shaders into shader objects
	result = g_pDevice->CreateVertexShader(vertexShaderBytecode.data(), vertexShaderBytecode.size(), NULL, &g_pVertexShader); // Create the vertex shader
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create vertex shader\n");
		return result;
	}

	result = g_pDevice->CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), NULL, &g_pPixelShader); // Create the pixel shader
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create pixel shader\n");
		return result;
	}

	// Set the shader objects
	g_pDeviceContext->VSSetShader(g_pVertexShader, 0, 0); 
	g_pDeviceContext->PSSetShader(g_pPixelShader, 0, 0); 

	ID3D11ShaderReflection* vShaderReflection = NULL;
	result = D3DReflect(vertexShaderBytecode.data(), vertexShaderBytecode.size(), IID_ID3D11ShaderReflection, (void**)&vShaderReflection); 
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to reflect vertex shader\n"); 
		return result;
	}

	D3D11_SHADER_DESC shaderDesc;
	vShaderReflection->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	D3D11_SIGNATURE_PARAMETER_DESC* signatureParamDesc = new D3D11_SIGNATURE_PARAMETER_DESC[shaderDesc.InputParameters]{ 0 };
	for (UINT i = 0; i < shaderDesc.InputParameters; i++)
	{
		vShaderReflection->GetInputParameterDesc(i, &signatureParamDesc[i]);
	}

	// Signature param desk masks as follows: float4 = 15, float3 = 7, float2 = 3, float[1] = 1. These are bitmasks
	// To put together input layout from shader info its something like this:
	D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[shaderDesc.InputParameters]{ 0 };
	for (size_t i = 0; i < shaderDesc.InputParameters; i++)
	{
		ied[i].SemanticName = signatureParamDesc[i].SemanticName;
		ied[i].SemanticIndex = signatureParamDesc[i].SemanticIndex;
		if (signatureParamDesc[i].ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
		{
			switch (signatureParamDesc[i].Mask)
			{
			case 1: ied[i].Format = DXGI_FORMAT_R32_FLOAT; break; // float 1
			case 3: ied[i].Format = DXGI_FORMAT_R32G32_FLOAT; break; // float 2
			case 7: ied[i].Format = DXGI_FORMAT_R32G32B32_FLOAT; break; // float 3
			case 15: ied[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break; // float 4
			default:
				break;
			}
		}// The above only covers ---x, --yx, -zyx, wzyx. It may be possible for a mask to be -yx- or yx- or zyx- (6, 12, 14)
		ied[i].InputSlot = 0;
		ied[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		ied[i].InstanceDataStepRate = 0;
	}

	result = g_pDevice->CreateInputLayout(ied, shaderDesc.InputParameters, vertexShaderBytecode.data(), vertexShaderBytecode.size(), &g_pInputLayout);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create input layout\n");
		return result;
	}

	g_pDeviceContext->IASetInputLayout(g_pInputLayout); 

	delete[] signatureParamDesc;
	delete[] ied;

	return S_OK;
}

HRESULT InitD3D(HWND hWnd)
{

	#pragma region Swapchain Info
	// Create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd = {};
	// Fill the swap chain description struct
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //32-bit color
	scd.BufferDesc.Width = GameWindow::GetWindowWidth();
	scd.BufferDesc.Height = GameWindow::GetWindowHeight();
	scd.BufferDesc.RefreshRate.Numerator = 60; // 60 FPS
	scd.BufferDesc.RefreshRate.Denominator = 1; // 60/1 = 60 FPS
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // How swap chain is to be used
	scd.OutputWindow = hWnd; // The window to be used
	scd.SampleDesc.Count = 1; // How many multisamples
	scd.Windowed = TRUE; // Windowed or full screen
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Allow full screen switching
	#pragma endregion	

	#pragma region Device that uses scd struct
	// Create a device, device context and swap chain using the information in the scd struct
	HRESULT hr;
	// Create a swap chain, device and device context from the scd
	hr = D3D11CreateDeviceAndSwapChain(NULL, // Use default graphics adapter
		D3D_DRIVER_TYPE_HARDWARE,			 // Use hardware graphics acceleration, can also use software or WARP renderers
		NULL,								 // Used for software driver types
		D3D11_CREATE_DEVICE_DEBUG,			 // Flags can be OR'd together. We are enabling debug for better warnings and errors
		NULL,								 // Direct3D feature levels- NULL will use D3D11.0 or older
		NULL,								 // Size of array passed to above member - NULL since we passed NULL
		D3D11_SDK_VERSION,					 // Always set this to D3D11_SDK_VERSION
		&scd,								 // Pointer to swap chain description
		&g_pSwapChain,						 // Pointer to our swap chain
		&g_pDevice,							 // Pointer to our Direct3D device
		NULL,								 // Out param - will be set to chosen feature level
		&g_pDeviceContext);					 // Pointer to our immediate device context
	if (FAILED(hr)) return hr;

	// Get the address of the back buffer
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	// Get the back buffer from the swap chain
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);
	if (FAILED(hr)) return hr;

	// Use the back buffer address to create the render target
	hr = g_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &g_backbuffer);
	if (FAILED(hr)) return hr;

	// Release the pointer to the back buffer as we no longer need it
	pBackBufferTexture->Release();

	// Set the render target as the back buffer
	g_pDeviceContext->OMSetRenderTargets(1, &g_backbuffer, NULL);
#pragma endregion	

#pragma region Viewport
	// Define and set the viewport
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0; // Top left x position of viewport
	viewport.TopLeftY = 0; // Top left y position of viewport
	viewport.Width = GameWindow::GetWindowWidth(); // Width of viewport
	viewport.Height = GameWindow::GetWindowHeight(); // Height of viewport
	viewport.MinDepth = 0; // Min depth of viewport
	viewport.MaxDepth = 1; // Max depth of viewport
	g_pDeviceContext->RSSetViewports(1, &viewport); // Set the viewport
#pragma endregion

	D3D11_BLEND_DESC bd1 = { 0 };
	bd1.RenderTarget[0].BlendEnable = TRUE;
	bd1.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd1.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd1.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd1.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd1.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd1.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd1.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd1.IndependentBlendEnable = FALSE;
	bd1.AlphaToCoverageEnable = FALSE;

	g_pDevice->CreateBlendState(&bd1, &pAlphaBlendEnable);

	D3D11_BLEND_DESC bd2 = { 0 };
	bd2.RenderTarget[0].BlendEnable = FALSE;
	bd2.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd2.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd2.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd2.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd2.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd2.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd2.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd2.IndependentBlendEnable = FALSE;
	bd2.AlphaToCoverageEnable = FALSE;

	g_pDevice->CreateBlendState(&bd2, &pAlphaBlendDisable);

	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FillMode = D3D11_FILL_SOLID;

	g_pDevice->CreateRasterizerState(&rsDesc, &pRasterizerState);

	g_pDeviceContext->RSSetState(pRasterizerState);

	//InitPipeline();

	return S_OK;
}

void InitGraphics()
{
	Vertex vertices[] =
	{
		/*{XMFLOAT3{-0.5f, -0.5f, 0.0f}, XMFLOAT4{Colors::LightGoldenrodYellow}},
		{XMFLOAT3{0.0f, 0.5f, 0.0f}, XMFLOAT4{Colors::MediumPurple}},
		{XMFLOAT3{0.5f, -0.5f, 0.0f}, XMFLOAT4{Colors::Turquoise}}*/

		{XMFLOAT3{-0.5f, -0.5f, -0.5f}, XMFLOAT4{Colors::MediumVioletRed}, XMFLOAT2{0.0f, 1.0f}},  // Front BL
		{XMFLOAT3{-0.5f,  0.5f, -0.5f}, XMFLOAT4{Colors::PaleVioletRed  }, XMFLOAT2{0.0f, 0.0f}},  // Front TL
		{XMFLOAT3{ 0.5f,  0.5f, -0.5f}, XMFLOAT4{Colors::MediumVioletRed}, XMFLOAT2{1.0f, 0.0f}},  // Front TR
		{XMFLOAT3{ 0.5f, -0.5f, -0.5f}, XMFLOAT4{Colors::PaleVioletRed  }, XMFLOAT2{1.0f, 1.0f}},  // Front BR

		{XMFLOAT3{-0.5f, -0.5f,  0.5f}, XMFLOAT4{Colors::PaleVioletRed  }, XMFLOAT2{0.0f, 1.0f}},  // Back BL
		{XMFLOAT3{-0.5f,  0.5f,  0.5f}, XMFLOAT4{Colors::MediumVioletRed}, XMFLOAT2{0.0f, 0.0f}},  // Back TL
		{XMFLOAT3{ 0.5f,  0.5f,  0.5f}, XMFLOAT4{Colors::PaleVioletRed  }, XMFLOAT2{1.0f, 0.0f}},  // Back TR
		{XMFLOAT3{ 0.5f, -0.5f,  0.5f}, XMFLOAT4{Colors::MediumVioletRed}, XMFLOAT2{1.0f, 1.0f}},  // Back BR

	};

	pText = new Text2D("Textures/font1.png", g_pDevice, g_pDeviceContext);

	// Create the vertex buffer
	D3D11_BUFFER_DESC bd = { 0 };
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);
	if (g_pVertexBuffer == 0)
	{
		OutputDebugString(L"Failed to create vertex buffer\n");
		return;
	}

	// Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	g_pDeviceContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // Map the buffer

	memcpy(ms.pData, vertices, sizeof(vertices)); // Copy the data 

	g_pDeviceContext->Unmap(g_pVertexBuffer, NULL); // Unmap the buffer


	unsigned int indices[] = { 0, 1, 2, 2, 3, 0, // Front face
							   7, 6, 5, 5, 4, 7, // Back face
							   4, 5, 1, 1, 0, 4, // Left face
							   3, 2, 6, 6, 7, 3, // Right face
                               1, 5, 6, 6, 2, 1, // Top face.
							   4, 0, 3, 3, 7, 4 }; // Bottom face
							   

	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(indices);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	// Define the resource data.
	D3D11_SUBRESOURCE_DATA InitData = { 0 };
	InitData.pSysMem = indices;

	if (FAILED(g_pDevice->CreateBuffer(&bufferDesc, &InitData, &pIBuffer)))
		OutputDebugString(L"Failed to create index buffer\n");



	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(CBUFFER0);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if(FAILED(g_pDevice->CreateBuffer(&cbd, NULL, &pCBuffer)))
		OutputDebugString(L"Failed to create constant buffer\n");


	CreateWICTextureFromFile(g_pDevice, g_pDeviceContext, L"Textures/Box.bmp", NULL, &pTexture);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap texture if U coordinate is outside of 0.0f to 1.0f
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap texture if V coordinate is outside of 0.0f to 1.0f
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap texture if W coordinate is outside of 0.0f to 1.0f
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Use all mipmap levels
	g_pDevice->CreateSamplerState(&samplerDesc, &pSampler); 

}

void RenderFrame() 
{	
	// Clear the back buffer the color parameter
	g_pDeviceContext->ClearRenderTargetView(g_backbuffer, clearColor);

	// Select which vertex buffer to use
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pDeviceContext->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R32_UINT, 0);


	// Select which primitive we are using
	g_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pos.z = 4.0f;

	static float fakeTime = 0.0f;
	fakeTime += 0.0001f;
	//pos.x = sin(fakeTime);
	//pos.y = cos(fakeTime);
	//rot.z = sin(fakeTime);
	//rot.y = cos(fakeTime);

	CBUFFER0 cBuffer0;
	XMMATRIX translation, rotation, scale;
	XMMATRIX world;
	XMMATRIX projection;
	XMMATRIX view;

	// View
	XMVECTOR eyePos = { g_camera.x, g_camera.y, g_camera.z };

	XMVECTOR lookTo = { sin(g_camera.yaw) * sin(g_camera.pitch), 
						cos(g_camera.pitch), 
						cos(g_camera.yaw) * sin(g_camera.pitch) };

	XMVECTOR campUp = { 0,1,0 }; // World up

	view = XMMatrixLookToLH(eyePos, lookTo, campUp);

	// Transform matrices
	translation = XMMatrixTranslation(pos.x, pos.y, pos.z);
	rotation = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	scale = XMMatrixScaling(scl.x, scl.y, scl.z);

	world = scale * rotation * translation;

	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60),GameWindow::GetWindowWidth() / (float)GameWindow::GetWindowHeight(), 0.1f, 100.0f);

	// Set cbuffer values
	cBuffer0.WVP = world * view * projection;

	g_pDeviceContext->UpdateSubresource(pCBuffer, 0, NULL, &cBuffer0, 0, 0);
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &pCBuffer);

	g_pDeviceContext->PSSetSamplers(0, 1, &pSampler);
	g_pDeviceContext->PSSetShaderResources(0, 1, &pTexture); 

	// Draw 3 vertices
	g_pDeviceContext->DrawIndexed(36, 0, 0);

	pText->AddText("Hello World", -1, +1, 0.075f);
	g_pDeviceContext->OMSetBlendState(pAlphaBlendEnable, NULL, 0xffffffff);
	pText->RenderText();
	g_pDeviceContext->OMSetBlendState(pAlphaBlendDisable, NULL, 0xffffffff);


	// Flip the back and front buffers around. Display on screen
	g_pSwapChain->Present(0, 0);

	
}

void CleanD3D()
{
	// Close and release all existing COM objects
	delete pText;
	if(pAlphaBlendEnable) pAlphaBlendEnable->Release();
	if(pAlphaBlendDisable) pAlphaBlendDisable->Release();
	if(pCBuffer) pCBuffer->Release();
	if(pVBuffer) pVBuffer->Release();
	if(g_backbuffer) g_backbuffer->Release();
	if(g_pSwapChain) g_pSwapChain->Release();
	if(g_pDevice) g_pDevice->Release();
	if(g_pDeviceContext) g_pDeviceContext->Release();
	if (g_pInputLayout) g_pInputLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (pTexture) pTexture->Release();
	if (pSampler) pSampler->Release();

}

void OpenConsole()
{
	if (AllocConsole())
	{
		// Redirecting input, output and error streams to the console
		FILE* fp = nullptr;
		freopen_s(&fp, "CONIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		std::ios::sync_with_stdio(true);

		std::cout << "Hello side console!!" << std::endl;
	}
}