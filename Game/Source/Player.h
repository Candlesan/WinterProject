#pragma once
#include "Component.h"
#include "Input.h"
#include "MoveComponent.h"
#include "CameraComponent.h"

// プレイヤー
class Player : public Component
{
public:
	Player() {};
	~Player() {};

	// 名前を取得
	const char* GetName() const override { return "Player"; }

	// 開始
	void Start() override;

	// 更新処理
	void Update(float elapsedTime) override;

private:
	// キャラクター操作
	void CharacterControl(float elapsedTime);

	DirectX::XMFLOAT3 GetMoveVec() const;
private:
	std::shared_ptr<MoveComponent> moveComponent;
	std::shared_ptr<CameraComponent> cameraComponent;
};

