#pragma once
#include "Component.h"


class WeaponComponent : public Component
{
public:
	WeaponComponent() {}
	~WeaponComponent() {}

	// 名前取得
	const char* GetName() const override { return "Weapon"; }

	// 開始処理
	void Start() override;

	// 更新処理
	void Update(float elpasedTime) override;

	// GUI描画
	void OnGUI() override;

	// 親アクターを設定
	void SetParentActor(std::shared_ptr<Actor> parent) { parentActor = parent; }
	void SetHitActor(std::shared_ptr<Actor> hitActor) { this->HitActor = hitActor; }
	std::weak_ptr<Actor>GetHitActor() const { return HitActor; }

	// アタッチ先のボーン名を設定
	void SetAttachBoneName(const char* name) { attachBoneName = name; }

	// 武器のローカル位置を設定
	void SetLocalPosition(const DirectX::XMFLOAT3& pos) { localPosition = pos; }
	void SetLocalRotation(const DirectX::XMFLOAT3& rot) { localRotation = rot; }
	void SetLocalScale(const DirectX::XMFLOAT3& scl) { localScale = scl; }

	// 攻撃判定
	void SetWeaponHitOffset(const DirectX::XMFLOAT3& offset) { weaponHitOffset = offset; }

	DirectX::XMFLOAT3 GetWeaponHitPosition() const;
	float GetWeaponHitRadius() const { return weaponHitRadius; }

private:
	void UpdateWeaponTransform();

private:
	std::weak_ptr<Actor> parentActor; // 親(キャラ)
	std::weak_ptr<Actor> HitActor; // 攻撃判定Actor
	std::string attachBoneName = "B_R_Hand";

	DirectX::XMFLOAT3		localPosition = { 0, 0, 0 };
	DirectX::XMFLOAT3		localRotation = { 0, 0, 0 };
	DirectX::XMFLOAT3		localScale = { 0.01f, 0.01f, 0.01f };
	DirectX::XMFLOAT3					weaponHitOffset = { 0, 0, 1.8f };
	DirectX::XMFLOAT3 hitLocalPosition = { 0, 0, 1.8f };

	float weaponHitRadius = 0.5f;
};