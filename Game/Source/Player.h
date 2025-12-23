#pragma once
#include "Input.h"
#include "Model.h"
#include "Component.h"
#include "MoveComponent.h"
#include "CameraComponent.h"
#include "HealthComponent.h"
#include "WeaponCollision.h"
#include "ShapeRenderer.h"

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

	// GUI描画
	void OnGUI() override;

	// デバックプリミティブ描画
	void RenderDebugPrimitive(ShapeRenderer* shapeRenderer);

private:
	// キャラクター操作
	void CharacterControl(float elapsedTime);

	// 武器のアタッチメント
	void WeaponAttachment(float elapsedTime);

	// 攻撃判定位置を取得
	DirectX::XMFLOAT3 GetWeaponHitPosition() const;

	// アニメーション更新処理
	void UpdateAnimation(float elapsedTime);

	std::shared_ptr<WeaponCollision> GetWeaponCollision();

	enum class State
	{
		Idle = 0,
		Walk,
		Run,
		Attack,
	};

private:
	std::shared_ptr<MoveComponent> moveComponent;
	std::shared_ptr<CameraComponent> cameraComponent;
	std::shared_ptr<HealthComponent> healthComponent;
	std::shared_ptr<WeaponCollision> weaponCollision;
	std::shared_ptr<Actor> weaponActor;

	// 武器関係
	std::shared_ptr<Model> weaponModel;
	DirectX::XMFLOAT3 localPosition = { -15.9, 13.4, 7.1 };
	DirectX::XMFLOAT3 localAngle = { 11.1, 11.1, 12.4 };
	DirectX::XMFLOAT3 localScale{ 1, 1, 1 };
	DirectX::XMFLOAT4X4 WeaponTransform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 
	};
	DirectX::XMFLOAT3 weaponHitOffset = { 16.5, -110.4, 0 };
	float weaponHitRadius = 0.5f;
	float weaponHitHeight = 1.0f;

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

