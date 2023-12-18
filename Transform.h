#pragma once
#include <DirectXMath.h>

class Transform
{
public:
	Transform();
	~Transform() = default;

	// Getters
	DirectX::XMFLOAT3 GetPosition() { return position; }
	DirectX::XMFLOAT3 GetRotation() { return rotation; }
	DirectX::XMFLOAT3 GetScale() { return scale; }

	void SetPosition(float x, float y, float z) { position = DirectX::XMFLOAT3(x, y, z); }
	void SetRotation(float x, float y, float z) { rotation = DirectX::XMFLOAT3(x, y, z); }
	void SetScale(float x, float y, float z) { scale = DirectX::XMFLOAT3(x, y, z); }


	//TODO - Uncomment when other classes are done
	//DirectX::XMMATRIX GetWorldMatrix();

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
};

