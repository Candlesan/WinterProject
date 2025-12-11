#pragma once
#include <DirectXMath.h>
#include "Component.h"

// 移動
class MoveComponent : public Component
{
public:
	MoveComponent() {};
	~MoveComponent() {};

	// 名前取得
	const char* GetName() const override { return "Move"; }

	// 更新
	void Update(float elapsedTime) override;

	// GUI描画
	void OnGUI() override;

	//移動処理
	void Move(float elapsedTime, float vx, float vz);

	//旋回処理
	//void Turn(float elapsedTime, float vx, float vz, float speed);
	void Turn(const DirectX::XMFLOAT3& direction, float elapsedTime);

	//ジャンプ処理
	void Jump();

	//速力処理更新
	void UpdateVelocity(float elapsedTime);

	//着地した時に呼ばれる
	virtual void OnLanding() {}

	//ダメージを受けた時に呼ばれる
	virtual void OnDamage() {}

	//死亡したと時に呼ばれる
	virtual void OnDead() {}

private:
	//垂直速力更新処理
	void UpdateVerticalVelocity(float elapsedTime);

	//垂直移動更新処理
	void UpdateVerticalMove(float elapsedTime);

	//水平速力更新処理
	void UpdateHorizontalVelocity(float elapsedTime);

	//水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);

private:
	float gravity = -30.0f;
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };

	bool isGround = false;

	float friction = 15.0f;
	float acceleration = 50.0f;
	float maxMoveSpeed = 5.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;
	float airControl = 0.3f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
	float moveSpeed = 5.0f;
	float jumpSpeed = 12.0f;
};
