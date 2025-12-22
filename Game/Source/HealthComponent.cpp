#include "HealthComponent.h"
#include <imgui.h>


// 更新処理
void HealthComponent::Update(float elapsedTIme)
{
	UpdateInvincbleTimer(elapsedTIme);
}

// GUI描画
void HealthComponent::OnGUI()
{
	ImGui::Text("Health : %f", health);
	ImGui::Text("MaxHealth : %f", maxHealth);
	ImGui::Text("InvincbleTimer : %f", invincbleTimer);
}

// ダメージ処理
bool HealthComponent::ApplyDamage(float damage, float invincbleTime)
{
	// ダメージが0以下の場合は何もしない
	if (damage <= 0) return false;
	// 既に死亡している場合は無視
	if (health <= 0) return false;
	// 無敵時間中ならダメージを受けない
	if (invincbleTimer > 0.0f) return false;

	// ダメージ適用
	health -= damage;
	invincbleTimer = invincbleTime; // 無敵時間設定

	if (health <= 0)
	{
		OnDead(); // 死亡通知
	}
	else
	{
		OnDamage(); // ダメージ通知
	}

	return true;
}

// 無敵時間更新
void HealthComponent::UpdateInvincbleTimer(float elapesdTime)
{
	if (invincbleTimer > 0.0f)
	{
		invincbleTimer -= elapesdTime;
	}
}
