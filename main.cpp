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

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include <Mouse.h>
#include <Keyboard.h>

#include "objfilemodel.h"


using namespace DirectX;

#define MAX_POINT_LIGHTS 8

#pragma region Global Variables

XMVECTORF32 clearColor = Colors::Teal; // Default clear color

// Direct3D Variables
IDXGISwapChain* g_pSwapChain = NULL; // The pointer to the swap chain interface
ID3D11Device* g_pDevice = NULL; // The pointer to our Direct3D device interface
ID3D11DeviceContext* g_devcon = NULL; // The pointer to our Direct3D device context
ID3D11RenderTargetView* g_backbuffer = NULL; // A view to access our back buffer

ID3D11DepthStencilView* g_ZBuffer = NULL; // The pointer to our depth buffer

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
	XMFLOAT3 normal;
};

ID3D11Buffer* pVBuffer = NULL; // Vertex
ID3D11Buffer* pIBuffer = NULL; // Index
ID3D11Buffer* pCBuffer = NULL; // Constant

struct PointLight
{
	XMVECTOR position = { 0,0,0,1 };
	XMVECTOR colour = { 1,1,1,1 };

	float strength = 10;
	BOOL enabled = false;
	float padding[2];
};

struct CBUFFER0
{
	XMMATRIX WVP; // 64 bytes World-View-Projection matrix
	XMVECTOR ambientLightCol;
	XMVECTOR directionalLightDir;
	XMVECTOR directionalLightCol;
	PointLight pointLights[MAX_POINT_LIGHTS];

};

struct CBufferSkybox
{
	XMMATRIX WVP;
};

// Transform
struct Transform
{
	XMFLOAT3 pos{ 0,0,2 };
	XMFLOAT3 rot{ 0,0,0 };
	XMFLOAT3 scl{ 1,1,1 };

	XMMATRIX GetWorldMatrix()
	{
		XMMATRIX translation = XMMatrixTranslation(pos.x, pos.y, pos.z);
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		XMMATRIX scale = XMMatrixScaling(scl.x, scl.y, scl.z);

		XMMATRIX world = scale * rotation * translation;
		return world;
	}
};

Transform cube1;
Transform cube2;

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

	XMMATRIX GetViewMatrix()
	{
		XMVECTOR eyePos{ x,y,z };
		XMVECTOR camUp{ 0,1,0 }; // World Up
		XMVECTOR lookTo{sin(yaw) * sin(pitch),
						cos(pitch),
						cos(yaw) * sin(pitch)};

		XMMATRIX view = XMMatrixLookToLH(eyePos, lookTo, camUp);
		return view;
	}
};

Camera g_camera;

ID3D11ShaderResourceView* pTexture = NULL; // Texture
ID3D11SamplerState* pSampler = NULL; // Sampler

Text2D* pText;

ID3D11BlendState* pAlphaBlendEnable = NULL;
ID3D11BlendState* pAlphaBlendDisable = NULL;

ID3D11RasterizerState* pRasterizerState = NULL;

Keyboard keyboard;
Keyboard::KeyboardStateTracker kbTracker;

Mouse mouse;
Mouse::ButtonStateTracker mbTracker;

XMVECTOR ambientLightColour = { 0.1f, 0.1f, 0.1f, 1.0f };
XMVECTOR directionalLightShinesFrom = { 0.2788f, 0.7063f, 0.6506f };
XMVECTOR directionalLightColour = { 0.96f, 0.8f, 0.75f, 1.0f };
PointLight pointLights[MAX_POINT_LIGHTS];

ObjFileModel* model;

ID3D11RasterizerState* pRasterSolid = NULL;
ID3D11RasterizerState* pRasterSkybox = NULL;
ID3D11DepthStencilState* pDepthWriteSolid = NULL;
ID3D11DepthStencilState* pDepthWriteSkybox = NULL;
ID3D11Buffer* pSkyboxCBuffer = NULL;
ID3D11ShaderResourceView* pSkyboxTexture = NULL;
ID3D11VertexShader* pVSSkybox = NULL;
ID3D11PixelShader* pPSSkybox = NULL;
ID3D11InputLayout* pLayoutSkybox = NULL;

#pragma endregion

#pragma region Function Properties
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

// Direct3D Functions
HRESULT InitD3D(HWND hWnd); // Sets up and initializes Direct3D
HRESULT LoadVertexShader(LPCWSTR filename, ID3D11VertexShader** vs, ID3D11InputLayout** il);
HRESULT LoadPixelShader(LPCWSTR filename, ID3D11PixelShader** ps);
void CleanD3D(); // Closes Direct3D and releases memory
void RenderFrame();

void DrawSkybox();

void InitGraphics(); // Create the shape to render
HRESULT InitPipeline(); // Loads and prepares the shaders

// Console window for debug output
void OpenConsole();

void InitScene();
void HandleInput();
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

	InitScene();

	Mouse::Get().SetWindow(GameWindow::GetWindowHandle());
	Mouse::Get().SetMode(Mouse::MODE_RELATIVE);

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
			HandleInput();
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
	
	case WM_DESTROY: // This message is sent when the user closes the window
		PostQuitMessage(0);
		return 0;
		break;

	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
	case WM_INPUT:
		Keyboard::ProcessMessage(message, wParam, lParam);
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		if(wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// This is where you'd implement the classic alt+enter fullscreen toggle code
		}
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;


	case WM_MOUSEACTIVATE:
		return MA_ACTIVATEANDEAT; // This will ignore mouse clicks that regain focus on the window
		break;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;

}

HRESULT InitPipeline()
{
	LoadVertexShader(L"Compiled Shaders/VertexShader.cso", &g_pVertexShader, &g_pInputLayout);
	LoadPixelShader(L"Compiled Shaders/PixelShader.cso", &g_pPixelShader);

	g_devcon->VSSetShader(g_pVertexShader, 0, 0);

	g_devcon->IASetInputLayout(g_pInputLayout);

	g_devcon->PSSetShader(g_pPixelShader, 0, 0);

	LoadVertexShader(L"Compiled Shaders/SkyboxVShader.cso", &pVSSkybox, &pLayoutSkybox);
	LoadPixelShader(L"Compiled Shaders/SkyboxPShader.cso", &pPSSkybox);

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
#pragma  endregion

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
		&g_devcon);					 // Pointer to our immediate device context
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

	D3D11_TEXTURE2D_DESC tex2dDesc = { 0 };
	tex2dDesc.Width = GameWindow::GetWindowWidth();
	tex2dDesc.Height = GameWindow::GetWindowHeight();
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = scd.SampleDesc.Count; // Same sample description as the swap chain
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* zBufferTexture;
	hr = g_pDevice->CreateTexture2D(&tex2dDesc, NULL, &zBufferTexture);
	if(FAILED(hr))
	{
		OutputDebugString(L"Failed to create depth stencil texture\n");
		return E_FAIL;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = g_pDevice->CreateDepthStencilView(zBufferTexture, &dsvDesc, &g_ZBuffer);
	if(FAILED(hr))
	{
		OutputDebugString(L"Failed to create depth stencil view\n");
		return E_FAIL;
	}
	zBufferTexture->Release();

	// Set the render target as the back buffer
	g_devcon->OMSetRenderTargets(1, &g_backbuffer, g_ZBuffer);
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
	g_devcon->RSSetViewports(1, &viewport); // Set the viewport
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

	// Backface culling
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FillMode = D3D11_FILL_SOLID;

	g_pDevice->CreateRasterizerState(&rsDesc, &pRasterSolid);

	// Frontface culling
	rsDesc.CullMode = D3D11_CULL_FRONT;
	g_pDevice->CreateRasterizerState(&rsDesc, &pRasterSkybox);

	// Depth writing enabled
	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	g_pDevice->CreateDepthStencilState(&dsDesc, &pDepthWriteSolid);

	// Depth writing disabled
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	g_pDevice->CreateDepthStencilState(&dsDesc, &pDepthWriteSkybox);

	g_devcon->RSSetState(pRasterizerState);

	return S_OK;

}

HRESULT LoadVertexShader(LPCWSTR filename, ID3D11VertexShader** vs, ID3D11InputLayout** il)
{
	HRESULT result;

	auto vertexShaderBytecode = DX::ReadData(filename);

	result = g_pDevice->CreateVertexShader(vertexShaderBytecode.data(),vertexShaderBytecode.size(), NULL, vs);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create vertex shader\n");
		return result;
	}

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

	result = g_pDevice->CreateInputLayout(ied, shaderDesc.InputParameters, vertexShaderBytecode.data(), vertexShaderBytecode.size(), il);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create input layout\n");
		return result;
	}

	//g_pDeviceContext->IASetInputLayout(g_pInputLayout);

	delete[] signatureParamDesc;
	delete[] ied;

	return S_OK;
}

HRESULT LoadPixelShader(LPCWSTR filename, ID3D11PixelShader** ps)
{
	HRESULT result;

	auto pixelShaderBytecode = DX::ReadData(filename);

	result = g_pDevice->CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), NULL, ps);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create pixel shader\n");
		return result;
	}
}

void InitGraphics()
{
	Vertex vertices[] =
	{
		{XMFLOAT3{-0.5f, -0.5f, -0.5f}, XMFLOAT4{Colors::MediumVioletRed}, XMFLOAT2{0.0f, 1.0f}, XMFLOAT3{-0.5773f, -0.5773f, -0.5773f}},  // Front BL
		{XMFLOAT3{-0.5f,  0.5f, -0.5f}, XMFLOAT4{Colors::PaleVioletRed  }, XMFLOAT2{0.0f, 0.0f}, XMFLOAT3{-0.5773f,  0.5773f, -0.5773f}},  // Front TL
		{XMFLOAT3{ 0.5f,  0.5f, -0.5f}, XMFLOAT4{Colors::MediumVioletRed}, XMFLOAT2{1.0f, 0.0f}, XMFLOAT3{ 0.5773f,  0.5773f, -0.5773f}},  // Front TR
		{XMFLOAT3{ 0.5f, -0.5f, -0.5f}, XMFLOAT4{Colors::PaleVioletRed  }, XMFLOAT2{1.0f, 1.0f}, XMFLOAT3{ 0.5773f, -0.5773f, -0.5773f}},  // Front BR

		{XMFLOAT3{-0.5f, -0.5f,  0.5f}, XMFLOAT4{Colors::PaleVioletRed  }, XMFLOAT2{0.0f, 1.0f}, XMFLOAT3{-0.5773f, -0.5773f,  0.5773f}},  // Back BL
		{XMFLOAT3{-0.5f,  0.5f,  0.5f}, XMFLOAT4{Colors::MediumVioletRed}, XMFLOAT2{0.0f, 0.0f}, XMFLOAT3{-0.5773f,  0.5773f,  0.5773f}},  // Back TL
		{XMFLOAT3{ 0.5f,  0.5f,  0.5f}, XMFLOAT4{Colors::PaleVioletRed  }, XMFLOAT2{1.0f, 0.0f}, XMFLOAT3{ 0.5773f,  0.5773f,  0.5773f}},  // Back TR
		{XMFLOAT3{ 0.5f, -0.5f,  0.5f}, XMFLOAT4{Colors::MediumVioletRed}, XMFLOAT2{1.0f, 1.0f}, XMFLOAT3{ 0.5773f, -0.5773f,  0.5773f}},  // Back BR

	};

	pText = new Text2D("Textures/font1.png", g_pDevice, g_devcon);

	model = new ObjFileModel{ (char*)"Models/cube.obj",g_pDevice,g_devcon };

	CreateDDSTextureFromFile(g_pDevice, g_devcon, L"skybox01.dds", NULL, &pSkyboxTexture);

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
	g_devcon->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // Map the buffer

	memcpy(ms.pData, vertices, sizeof(vertices)); // Copy the data 

	g_devcon->Unmap(g_pVertexBuffer, NULL); // Unmap the buffer


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

	cbd.ByteWidth = sizeof(CBufferSkybox);

	if (FAILED(g_pDevice->CreateBuffer(&cbd, NULL, &pSkyboxCBuffer)))
	{
		OutputDebugString(L"Failed to create skybox constant buffer\n");
	}


	CreateWICTextureFromFile(g_pDevice, g_devcon, L"Textures/Box.bmp", NULL, &pTexture);

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

	//static double t = 0.0f;
	//t += 0.0001f;
	//cube1.pos.x = sin(t);
	//cube1.pos.y = cos(t) * 0.75;
	//cube1.pos.z = sin(t) * 0.75;

	// Clear the back buffer the color parameter
	g_devcon->ClearRenderTargetView(g_backbuffer, clearColor);
	g_devcon->ClearDepthStencilView(g_ZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DrawSkybox();

	// Select which vertex buffer, index buffer and primtive topology to use - PER MESH!!
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_devcon->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_devcon->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 	// Select which primitive we are using

	// View and projection matrices
	XMMATRIX world, view, projection;
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), GameWindow::GetWindowWidth() / (float)GameWindow::GetWindowHeight(), 0.1f, 100);
	view = g_camera.GetViewMatrix();

	CBUFFER0 cBuffer;

	world = cube1.GetWorldMatrix();
	cBuffer.WVP = world * view * projection;

	// Lighting
	// Ambient
	cBuffer.ambientLightCol = ambientLightColour;
	// Directional
	cBuffer.directionalLightCol = directionalLightColour;
	XMMATRIX transpose = XMMatrixTranspose(world);
	cBuffer.directionalLightDir = XMVector3Transform(directionalLightShinesFrom, transpose);
	// Point Light
	for (size_t i = 0; i< MAX_POINT_LIGHTS; ++i)
	{
		if(!pointLights[i].enabled)
			continue;

		XMMATRIX inverse = XMMatrixInverse(nullptr, world);

		cBuffer.pointLights[i].position = XMVector3Transform(pointLights[i].position, inverse);
		cBuffer.pointLights[i].colour = pointLights[i].colour;
		cBuffer.pointLights[i].strength = pointLights[i].strength;
		cBuffer.pointLights[i].enabled = pointLights[i].enabled;

	}

	// Update constant buffer
	g_devcon->UpdateSubresource(pCBuffer, 0, NULL, &cBuffer, 0, 0);
	g_devcon->VSSetConstantBuffers(0, 1, &pCBuffer);

	// Textures
	g_devcon->PSSetSamplers(0, 1, &pSampler);
	g_devcon->PSSetShaderResources(0, 1, &pTexture); 

	//g_pDeviceContext->DrawIndexed(36, 0, 0);
	model->Draw();

	//world = cube2.GetWorldMatrix();
	//cBuffer.WVP = world * view * projection;
	//g_devcon->UpdateSubresource(pCBuffer, 0, NULL, &cBuffer, 0, 0);
	//g_devcon->VSSetConstantBuffers(0, 1, &pCBuffer);
	//g_devcon->DrawIndexed(36, 0, 0);


	pText->AddText("Hello World", -1, +1, 0.075f);
	g_devcon->OMSetBlendState(pAlphaBlendEnable, NULL, 0xffffffff);
	pText->RenderText();
	g_devcon->OMSetBlendState(pAlphaBlendDisable, NULL, 0xffffffff);


	// Flip the back and front buffers around. Display on screen
	g_pSwapChain->Present(0, 0);

	
}

void DrawSkybox()
{
	// Front-face culling and disable depth write
 	g_devcon->OMSetDepthStencilState(pDepthWriteSkybox, 1);
	g_devcon->RSSetState(pRasterSkybox);

	// Set skybox shaders
	g_devcon->VSSetShader(pVSSkybox, 0, 0);
	g_devcon->PSSetShader(pPSSkybox, 0, 0);
	g_devcon->IASetInputLayout(pLayoutSkybox);

	// Constant buffer data
	CBufferSkybox cbuf;
	XMMATRIX translation, projection, view;

	translation = XMMatrixTranslation(g_camera.x, g_camera.y, g_camera.z);
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), GameWindow::GetWindowWidth() / (float)GameWindow::GetWindowHeight(), 0.1f, 100);
	view = g_camera.GetViewMatrix();
	cbuf.WVP = translation * view * projection;
	g_devcon->UpdateSubresource(pSkyboxCBuffer, 0, 0, &cbuf, 0, 0);

	// Set shader resources
	g_devcon->VSSetConstantBuffers(0, 1, &pSkyboxCBuffer);
	g_devcon->PSSetSamplers(0, 1, &pSampler);
	g_devcon->PSSetShaderResources(0, 1, &pSkyboxTexture);

	model->Draw();

	// Back-face culling and enable depth write
	g_devcon->OMSetDepthStencilState(pDepthWriteSolid, 1);
	g_devcon->RSSetState(pRasterSolid);

	// Set standard shaders
	g_devcon->VSSetShader(g_pVertexShader, 0, 0);
	g_devcon->PSSetShader(g_pPixelShader, 0, 0);
	g_devcon->IASetInputLayout(g_pInputLayout);
}

void CleanD3D()
{
	// Close and release all existing COM objects
	delete pText;
	delete model;
	if(pRasterSolid) pRasterSolid->Release();
	if(pRasterSkybox) pRasterSkybox->Release();
	if(pDepthWriteSolid) pDepthWriteSolid->Release();
	if(pDepthWriteSkybox) pDepthWriteSkybox->Release();
	if(pSkyboxCBuffer) pSkyboxCBuffer->Release();
	if(pSkyboxTexture) pSkyboxTexture->Release();
	if(pVSSkybox) pVSSkybox->Release();
	if(pPSSkybox) pPSSkybox->Release();
	if(g_ZBuffer) g_ZBuffer->Release();
	if(pAlphaBlendEnable) pAlphaBlendEnable->Release();
	if(pAlphaBlendDisable) pAlphaBlendDisable->Release();
	if(pCBuffer) pCBuffer->Release();
	if(pVBuffer) pVBuffer->Release();
	if(g_backbuffer) g_backbuffer->Release();
	if(g_pSwapChain) g_pSwapChain->Release();
	if(g_pDevice) g_pDevice->Release();
	if(g_devcon) g_devcon->Release();
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

void InitScene()
{
	cube1.pos = XMFLOAT3(0.7f, 0.0f, 3.0f);
	cube2.rot = XMFLOAT3(0.0f, XMConvertToRadians(45), 0.0f);

	pointLights[0] = { XMVECTOR{1.5f,0,-1}, {0.9,0,0.85f},10,true };
	pointLights[1] = { XMVECTOR{-1.5f,0,-1}, {0,0.9,0.85f},20,true };
}

void HandleInput()
{
	auto kbState = Keyboard::Get().GetState();
	kbTracker.Update(kbState);

	//if (kbTracker.pressed.Space)
	//	cout << "Space Pressed" << endl;

	//if (kbTracker.released.Space)
	//	cout << "Space Released" << endl;

	//if (kbTracker.lastState.Space)
	//	cout << "Space Held" << endl;

	if(kbState.Escape)
		PostQuitMessage(0);

	if(kbState.W)
	{
		g_camera.x += XMVectorGetX(g_camera.ForwardVector()) * 0.0025f;
		g_camera.z += XMVectorGetZ(g_camera.ForwardVector()) * 0.0025f;
	}

	if(kbState.S)
	{
		g_camera.x -= XMVectorGetX(g_camera.ForwardVector()) * 0.0025f;
		g_camera.z -= XMVectorGetZ(g_camera.ForwardVector()) * 0.0025f;
	}

	if(kbState.A)
	{
		g_camera.x += XMVectorGetX(g_camera.RightVector()) * 0.0025f;
		g_camera.z += XMVectorGetZ(g_camera.RightVector()) * 0.0025f;
	}

	if(kbState.D)
	{
		g_camera.x -= XMVectorGetX(g_camera.RightVector()) * 0.0025f;
		g_camera.z -= XMVectorGetZ(g_camera.RightVector()) * 0.0025f;
	}


	auto msState = Mouse::Get().GetState();
	mbTracker.Update(msState);

	float sensivity = XM_2PI * 0.00025f;
	g_camera.yaw += msState.x * sensivity;
	g_camera.pitch += msState.y * sensivity;

	if(mbTracker.leftButton == Mouse::ButtonStateTracker::PRESSED) // Can be HELD, RELEASED, PRESSED
	{
		// Reset camera
		g_camera.x = 0.0f;
		g_camera.y = 0.0f;
		g_camera.z = 0.0f;
		g_camera.pitch = XM_PIDIV2;
		g_camera.yaw = 0.0f;
	}


}