#pragma once
#include "Model.h"
#include "Component.h"
#include "MoveComponent.h"
#include "BehaviorTree.h"
#include "BehaviorData.h"
#include "NodeBase.h"
#include "ShapeRenderer.h"
#include "WeaponCollision.h"

// エネミー
class Enemy : public Component
{
public:
	Enemy() {};
	~Enemy() {};

	// 名前を取得
	const char* GetName() const override { return "Enemy"; }

	// 開始
	void Start() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// GUI描画
	void OnGUI() override;

	// アニメーション更新
	void UpdateAnimation(float elapsedTime);

	enum class State
	{
		Idle = 0,
		Walk,
		Run,
		Attack
	};

	// 縄張り設定
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

	// ターゲット位置をランダム設定
	void SetRandomTargetPosition();

	// 目標地点へ移動
	void MoveToTarget(float elapsedTime, float speedRate);

	// プレイヤー索敵
	bool SearchPlayer();

	// ターゲットポジション設定
	void SetTargetPosition(DirectX::XMFLOAT3 position) { targetPosition = position; }

	// ターゲットポジション取得
	DirectX::XMFLOAT3 GetTargetPosition() { return targetPosition; }

	// 攻撃範囲取得
	float GetAttackRange() { return attackRange; }

	// 実行タイマー設定
	void SetRunTimer(float timer) { runTimer = timer; }

	// 実行タイマー取得
	float GetRunTimer() { return runTimer; }

	void SetState(State index) { state = index; }

	// アニメーションが終了したか
	bool FinshedAnimation();

	//void SetHealth(int setHealth) { health = setHealth; }

private:
	void RenderDebugPrimitive(ShapeRenderer* shaprenderer);

	std::shared_ptr<WeaponCollision> GetWeaponCollision();
private:
	// コンポーネント
	std::shared_ptr<MoveComponent> moveComponent;
	std::shared_ptr<WeaponCollision> weaponCollision;
	std::shared_ptr<Actor> weaponActor;

	DirectX::XMFLOAT3 weaponHitOffset = { 0, 0, 0 }; // 手からのズレ
	float weaponHitRadius = 0.5f;                   // 判定の大きさ

	// ビヘイビアツリー関係
	DirectX::XMFLOAT3	targetPosition = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3	territoryOrigin = { 0.0f,0.0f,0.0f };
	float				territoryRange = 10.0f;
	float				moveSpeed = 3.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(360);
	float				searchRange = 5.0f;
	float				attackRange = 1.5f;
	float				runTimer = 0.0f;

	BehaviorTree* aiTree = nullptr;
	BehaviorData* behaviorData = nullptr;
	NodeBase* activeNode = nullptr;

	// アニメーション関係
	State state = State::Idle;

	DirectX::XMFLOAT4X4					worldTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	std::vector<Model::NodePose> nodePoses;		// 現在の姿勢
	std::vector<Model::NodePose> oldNodePoses;	// 前フレームの姿勢
	int animationIndex = -1;					// 現在のアニメーション
	float animationSeconds = 0;					// 現在の再生時間
	float oldAnimationSeconds = 0;				// 前フレームの再生時間
	float animationBlendSeconds = 0.2f;			// 補間する時間
	float animationBlendSecondsLength = 0.2f;	// 補間する長さ
	bool isBlending = true;						// アニメーションの補間をするか
	bool animationLoop = false;					// ループするか
	bool useRootMotion = false;					// ルートモーションをするか
	bool bakeTranslationY = false;				// Y軸を無視するか
};
