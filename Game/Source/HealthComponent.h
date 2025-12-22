#pragma once
#include "Component.h"

class HealthComponent : public Component
{
public:
	HealthComponent() {}
	~HealthComponent() {}

	// 名前を取得
	const char* GetName() const override { return "Health"; }

	// 更新処理
	void Update(float elapsedTIme) override;

	// GUI描画
	void OnGUI() override;

	// ダメージ処理
	bool ApplyDamage(float damage, float invincbleTime);

	// ゲッター・セッター
	float GetHealth() const { return health; }
	void SetMaxHealth(float max) { maxHealth = max; }
	void SetHealth(float hp) { health = (hp > maxHealth) ? maxHealth : hp; }

protected:
	virtual void OnDamage() {}
	virtual void OnDead() {}

private:
	void UpdateInvincbleTimer(float elapesdTime);

private:
	float health = 5.0f;
	float maxHealth = 5.0f;
	float invincbleTimer = 0.0f;
};
