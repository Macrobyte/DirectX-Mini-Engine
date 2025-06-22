#include "Skybox.h"
#include "../GameObjects/Components/Material.h"
#include "../Core/Game/World.h"
#include "../Rendering/Camera.h"
#include "../GameObjects/Components/Mesh.h"

Skybox::Skybox(Mesh* mesh, Material* material)
{
	this->mesh = mesh;
	this->material = material;
}

Skybox::~Skybox()
{
}

void Skybox::Render(ID3D11DeviceContext* devcon, DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
{
	CBufferUnlit cBuffer;
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(World::GetCamera()->GetPosition().x, World::GetCamera()->GetPosition().y, World::GetCamera()->GetPosition().z);
	cBuffer.WVP = translation * view * projection;
	devcon->UpdateSubresource(material->cBuffer, 0, nullptr, &cBuffer, 0, 0);

	material->SetActive(devcon);
	mesh->Draw();
}

