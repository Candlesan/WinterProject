#include "ActionDerived.h"
#include "Player.h"
#include "Mathf.h"

// 待機行動
ActionBase::State IdleAction::Run(float elapsedTime)
{
	float runTimer = owner->GetRunTimer();
	switch (step)
	{
	case 0:
		owner->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));
		owner->SetState(Enemy::State::Idle);
		step++;
		break;
	case 1:
		runTimer -= elapsedTime;
		// タイマー更新
		owner->SetRunTimer(runTimer);

		// 待機時間が過ぎた時
		if (runTimer <= 0.0f)
		{
			owner->SetRandomTargetPosition();
			step = 0;
			return ActionBase::State::Complete;
		}

		// プレイヤーを見つけた時
		if (owner->SearchPlayer())
		{
			step = 0;
			return ActionBase::State::Complete;
		}
		break;
	}
	// 実行中を返す
	return ActionBase::State::Run;
}

// 徘徊行動
ActionBase::State WanderAction::Run(float elapsedTime)
{
	// プレイヤーとの距離を計算
	std::shared_ptr<Actor> playerActor = ActorManager::Instance().FindActorName("Player");

	// プレイヤーの位置
	DirectX::XMFLOAT3 playerPos = playerActor->GetPosition();

	switch (step)
	{
	case 0:
		// 徘徊モーション設定
		owner->SetState(Enemy::State::Walk);
		step++;
		break;
	case 1:
		// 目的地点までのXZ平面での距離判定
		DirectX::XMFLOAT3 position = owner->GetActor()->GetPosition();
		DirectX::XMFLOAT3 targetPosition = playerPos;
		float vx = targetPosition.x - position.x;
		float vz = targetPosition.z - position.z;
		float distSq = vx * vx + vz * vz;

		// 目的地へ着いた
		float radius = 1.5f;
		if (distSq < radius * radius)
		{
			step = 0;
			// 徘徊成功を返す
			return ActionBase::State::Complete;
		}

		// 目的地点へ移動
		owner->MoveToTarget(elapsedTime, 0.5f);

		// プレイヤー索敵成功したら
		if (owner->SearchPlayer())
		{
			step = 0;
			// 徘徊成功を返す
			return ActionBase::State::Complete;
		}
		break;
	}
	// 実行中を返す
	return ActionBase::State::Run;

}

// 攻撃行動
ActionBase::State AttackAction::Run(float elapsedTime)
{
	switch (step)
	{
	case 0:
		owner->SetState(Enemy::State::Attack);
		step++;
		break;
	case 1:
		if (owner->FinshedAnimation())
		{
			step = 0;
			return ActionBase::State::Complete;
		}
	}
	// 実行中を返す
	return ActionBase::State::Run;
}

// 追跡行動
ActionBase::State PursuitAction::Run(float elapsedTime)
{
	float runTimer = owner->GetRunTimer();
	// プレイヤーとの距離を計算
	std::shared_ptr<Actor> playerActor = ActorManager::Instance().FindActorName("Player");

	// プレイヤーの位置
	DirectX::XMFLOAT3 playerPos = playerActor->GetPosition();


	switch (step)
	{
	case 0:
		// 目標地点をプレイヤー位置に設定
		owner->SetTargetPosition(playerPos);
		owner->SetRunTimer(Mathf::RandomRange(3.0f, 5.0f));
		owner->SetState(Enemy::State::Walk);
		step++;
		break;
	case 1:
	{
		runTimer -= elapsedTime;
		// タイマー更新
		owner->SetRunTimer(runTimer);
		// 目標地点をプレイヤー位置に設定
		owner->SetTargetPosition(playerPos);
		// 目的地点へ移動
		owner->MoveToTarget(elapsedTime, 1.0);

		DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();

		float vx = targetPosition.x - playerPos.x;
		float vy = targetPosition.y - playerPos.y;
		float vz = targetPosition.z - playerPos.z;
		float dist = sqrtf(vx * vx + vy * vy + vz * vz);
		// 攻撃範囲にいるとき
		if (dist < owner->GetAttackRange())
		{
			step = 0;
			// 追跡成功を返す
			return ActionBase::State::Complete;
		}
		// 行動時間が過ぎた時
		if (runTimer <= 0.0f)
		{
			step = 0;
			// 追跡失敗を返す
			return ActionBase::State::Failed;
		}
		break;
	}
	}
	// 実行中を返す
	return ActionBase::State::Run;

}