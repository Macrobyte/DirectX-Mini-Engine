#include "Material.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

Material::Material(ID3D11Device* device, LPCWSTR vsFilename, LPCWSTR psFilename, const wchar_t* textureFilename, CBuffers cbType, bool isSkybox)
{
	this->cBufferType = cbType;

	CreateConstantBuffer(device, cBufferType);
	LoadShaders(device, vsFilename, psFilename);

	texture = new Texture();
	reflectionTexture = new Texture();

	LoadTexture(textureFilename);

	if (isSkybox)
	{
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
		rasterDesc.CullMode = D3D11_CULL_FRONT;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		device->CreateRasterizerState(&rasterDesc, &rasterState);

		D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		device->CreateDepthStencilState(&dsDesc, &depthWrite);
	}
	else
	{
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		device->CreateRasterizerState(&rasterDesc, &rasterState);

		D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		device->CreateDepthStencilState(&dsDesc, &depthWrite);
	}
}

Material::Material(ID3D11Device* device, LPCWSTR vsFilename, LPCWSTR psFilename, const wchar_t* textureFilename, CBuffers cbType, const wchar_t* reflectTexFilename, bool isSkybox)
	: Material(device, vsFilename, psFilename, textureFilename, cbType, isSkybox)
{
	LoadReflectionTexture(reflectTexFilename);
}

Material::~Material()
{
	cBuffer->Release();
	vertexShader->Release();
	pixelShader->Release();
	inputLayout->Release();
	if(texture->shaderResource) texture->shaderResource->Release();
	if(texture->samplerState) texture->samplerState->Release();
	depthWrite->Release();
	rasterState->Release();
}

void Material::SetActive(ID3D11DeviceContext* devcon)
{
	devcon->VSSetShader(vertexShader, 0, 0);
	devcon->PSSetShader(pixelShader, 0, 0);
	devcon->IASetInputLayout(inputLayout);
	devcon->PSSetShaderResources(0, 1, &texture->shaderResource);
	devcon->PSSetSamplers(0, 1, &texture->samplerState);
	devcon->VSSetConstantBuffers(0, 1, &cBuffer);
	devcon->RSSetState(rasterState);
	devcon->OMSetDepthStencilState(depthWrite, 0);

	if(cBufferType == CB_Reflective)
		devcon->PSSetShaderResources(1, 1, &reflectionTexture->shaderResource);

}

void Material::LoadTexture(const wchar_t* filename)
{
	HRESULT hr = S_OK;

	// check last characters of wchar_t array to see if it's a DDS file or not
	if (filename[wcslen(filename) - 1] == 's' && filename[wcslen(filename) - 2] == 'd' && filename[wcslen(filename) - 3] == 'd')
		hr = DirectX::CreateDDSTextureFromFile(Renderer::GetInstance().GetDevice(), Renderer::GetInstance().GetDeviceContext(), filename, 0, &texture->shaderResource);
	else
		hr = DirectX::CreateWICTextureFromFile(Renderer::GetInstance().GetDevice(), Renderer::GetInstance().GetDeviceContext(), filename, 0, &texture->shaderResource, 0);

	if (FAILED(hr))
	{
		MessageBeep(MB_ICONSTOP);
		MessageBox(NULL, L"Failed to load texture", L"Critical Error!", MB_ICONERROR | MB_OK);
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; // Use anisotropic filtering
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap texture if U coordinate is outside of 0.0f to 1.0f
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap texture if V coordinate is outside of 0.0f to 1.0f
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // Wrap texture if W coordinate is outside of 0.0f to 1.0f
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Use all mipmap levels


	if (FAILED(Renderer::GetInstance().GetDevice()->CreateSamplerState(&samplerDesc, &texture->samplerState)))
	{
		MessageBeep(MB_ICONSTOP);
		MessageBox(NULL, L"Failed to create sampler state", L"Critical Error!", MB_ICONERROR | MB_OK);
	}

}

void Material::LoadReflectionTexture(const wchar_t* filename)
{
	HRESULT hr = S_OK;

	hr = DirectX::CreateDDSTextureFromFile(Renderer::GetInstance().GetDevice(), Renderer::GetInstance().GetDeviceContext(), filename, 0, &reflectionTexture->shaderResource);

	if (FAILED(hr))
	{
		MessageBeep(MB_ICONSTOP);
		MessageBox(NULL, L"Failed to load texture", L"Critical Error!", MB_ICONERROR | MB_OK);
	}
}

void Material::LoadShaders(ID3D11Device* device, LPCWSTR vsFilename, LPCWSTR psFilename)
{
	Renderer::GetInstance().LoadVertexShader(vsFilename, &vertexShader, &inputLayout);
	Renderer::GetInstance().LoadPixelShader(psFilename, &pixelShader);
}

void Material::CreateConstantBuffer(ID3D11Device* device, CBuffers cbType)
{

	UINT size = 0;
	switch (cbType)
	{
		case CB_Lit:
			size = sizeof(CBufferLit);
		break;

		case CB_Unlit:
			size = sizeof(CBufferUnlit);
		break;

		case CB_Reflective:
			size = sizeof(CBufferReflective);
		break;
	}

	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = size;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if (FAILED(device->CreateBuffer(&cbd, NULL, &cBuffer)))
		OutputDebugString(L"Failed to create constant buffer\n");
}
