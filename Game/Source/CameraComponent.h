#pragma once
#include "Component.h"
#include "Camera.h"
#include "Input.h"

class CameraComponent : public Component
{
public:
	CameraComponent() {}
	~CameraComponent() {}

	// –¼‘O‚ğæ“¾
	const char* GetName() const override { return "CameraComponent"; }

	void Start() override;

	void Update(float elapsedTime) override;

	void OnGUI() override;

	void SetTarget(std::shared_ptr<Actor> target) { this->target = target; }
	void SetOffset(const DirectX::XMFLOAT3& offset) { this->offset = offset; }
	void SetDistance(float dist) { this->distance = dist; }

private:
	std::shared_ptr<Actor> target;
	DirectX::XMFLOAT3 offset = { 0, 1.8, 0 };
	DirectX::XMFLOAT3 angle = { 0, 0, 0 };
	float distance = 5.0f;

	// ƒJƒƒ‰‚Ì‰ñ“]Šp‚ÌÅ‘å’l
	float rollSpeed = DirectX::XMConvertToRadians(90);
	float maxPitch = DirectX::XMConvertToRadians(45);
	float minPitch = DirectX::XMConvertToRadians(-45);
};
