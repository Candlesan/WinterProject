#include "JudgmentDerived.h"
#include "Player.h"
#include "Mathf.h"

// BattleNodeに遷移できるか判定
bool BattleJudgment::Judgment()
{
	// プレイヤーを見つけているか
	if (owner->SearchPlayer())
	{
		return true;
	}
	return false;
}

// AttackNodeに遷移できるか判定
bool AttackJudgment::Judgment()
{
	// プレイヤーとの距離を計算
	std::shared_ptr<Actor> playerActor = ActorManager::Instance().FindActorName("Player");

	// 対象との距離を算出
	DirectX::XMFLOAT3 position = owner->GetActor()->GetPosition();
	DirectX::XMFLOAT3 targetPosition = playerActor->GetPosition();

	float vx = targetPosition.x - position.x;
	float vy = targetPosition.y - position.y;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < owner->GetAttackRange())
	{
		// AttackNodeへ遷移できる
		return true;
	}
	return false;
}

// WanderNodeに遷移できるか判定
bool WanderJudgment::Judgment()
{
	// 目的地点までのXZ平面での距離判定
	DirectX::XMFLOAT3 position = owner->GetActor()->GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	// 目的地から離れている場合
	float radius = owner->GetActor()->GetRadius();
	if (distSq > radius * radius)
	{
		return true;
	}

	return false;
}
