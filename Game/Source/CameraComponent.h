#pragma once
#include "Component.h"
#include "Camera.h"
#include "Input.h"

class CameraComponent : public Component
{
public:
	CameraComponent() {}
	~CameraComponent() {}

	// 名前を取得
	const char* GetName() const override { return "CameraComponent"; }

	void Start() override;

	void Update(float elapsedTime) override;

	void OnGUI() override;

	void SetTarget(std::shared_ptr<Actor> target) { this->target = target; }
	void SetOffset(const DirectX::XMFLOAT3& offset) { this->offset = offset; }
	void SetDistance(float dist) { this->distance = dist; }

private:
	std::weak_ptr<Actor> target; 
	DirectX::XMFLOAT3 offset = { 0, 1.8, 0 };
	DirectX::XMFLOAT3 angle = { 0, 0, 0 };
	float distance = 5.0f;

	// カメラの回転角の最大値
	float rollSpeed = DirectX::XMConvertToRadians(90);
	float maxPitch = DirectX::XMConvertToRadians(45);
	float minPitch = DirectX::XMConvertToRadians(-45);
};

// memo
//std::shared_ptr<Actor> target;  // ★これが犯人!★
//```
//
//## 問題: 循環参照が発生してる
//```
//Player Actor(shared_ptr)
//↓ 所有
//CameraComponent(shared_ptr)
//↓ 所有
//Player Actor(shared_ptr)  ←★ループ!★

//なぜ weak_ptr を使うのか?
//shared_ptr の場合(問題あり)
//cppActor(参照カウント: 2)
//↑ CameraComponent が + 1
//↑ ActorManager が + 1
//
//// Actor を削除しようとしても...
//ActorManager が手放す → 参照カウント : 1
//CameraComponent がまだ持ってる!
//→ Actor は削除されない
//→ CameraComponent も削除されない
//→ メモリリーク!
//weak_ptr の場合(正常)
//cppActor(参照カウント: 1)
//↑ ActorManager が + 1
//↑ CameraComponent は参照カウントに影響しない
//
//// Actor を削除
//ActorManager が手放す → 参照カウント : 0
//→ Actor は削除される
//→ CameraComponent も削除される
//→ OK!
