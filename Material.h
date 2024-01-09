#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Renderer.h"
#include "SimpleMath.h"
#include "PointLight.h"

struct CBufferUnlit
{
	DirectX::XMMATRIX WVP; // Use for skybox
};


struct CBufferLit
{
	DirectX::XMMATRIX WVP; // 64 bytes World-View-Projection matrix
	DirectX::XMVECTOR ambientLightCol;
	DirectX::XMVECTOR directionalLightDir;
	DirectX::XMVECTOR directionalLightCol;
	//PointLight pointLights[MAX_POINT_LIGHTS]; // TODO - Add point lights
	PointLight pointLight;

	DirectX::SimpleMath::Vector2 tiling;
	float padding[2];
};

struct CBufferReflective
{
	DirectX::XMMATRIX WVP; // 64 bytes World-View-Projection matrix
	DirectX::XMMATRIX WV;
	DirectX::XMVECTOR ambientLightCol;
	DirectX::XMVECTOR directionalLightDir;
	DirectX::XMVECTOR directionalLightCol;
	PointLight pointLights[MAX_POINT_LIGHTS]; // TODO - Add point lights
};

enum CBuffers
{
	CB_Lit,
	CB_Unlit,
	CB_Reflective
};

struct Texture
{
	Texture() 
	{
		shaderResource = nullptr;
		samplerState = nullptr;
		tiling = { 1,1 };
	};

	ID3D11ShaderResourceView* shaderResource;
	ID3D11SamplerState* samplerState;
	DirectX::SimpleMath::Vector2 tiling;
};


class Material
{
public:
	Material(ID3D11Device* device, LPCWSTR vsFilename, LPCWSTR psFilename, const wchar_t* textureFilename, CBuffers cbType, bool isSkybox = false);
	Material(ID3D11Device* device, LPCWSTR vsFilename, LPCWSTR psFilename, const wchar_t* textureFilename, CBuffers cbType, const wchar_t* reflectTexFilename, bool isSkybox = false);
	~Material();


	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;

	Texture* texture = nullptr;
	Texture* reflectionTexture = nullptr;

	ID3D11DepthStencilState* depthWrite = nullptr;
	ID3D11RasterizerState* rasterState = nullptr;

	CBuffers cBufferType;

	ID3D11Buffer* cBuffer;

	void SetActive(ID3D11DeviceContext* devcon);

	Texture* GetTexture() { return texture; }

private:

	// Load texture WIC and DDS - move from InitGraphics
	void LoadTexture(const wchar_t* filename);
	void LoadReflectionTexture(const wchar_t* filename);

	// Load Shaders
	void LoadShaders(ID3D11Device* device, LPCWSTR vsFilename, LPCWSTR psFilename);

	void CreateConstantBuffer(ID3D11Device* device, CBuffers cbType);


};

