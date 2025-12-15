#pragma once
#include "Input.h"
#include "Model.h"
#include "Component.h"
#include "MoveComponent.h"
#include "CameraComponent.h"

// プレイヤー
class Player : public Component
{
public:
	Player() {};
	~Player();

	// 名前を取得
	const char* GetName() const override { return "Player"; }

	// 開始
	void Start() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// アニメーション更新処理
	void UpdateAnimation(float elapsedTime);

	enum class State
	{
		Idle = 0,
		Walk,
		Run,
		Attack,
	};

private:
	// キャラクター操作
	void CharacterControl(float elapsedTime);

	DirectX::XMFLOAT3 GetMoveVec() const;
private:
	std::shared_ptr<MoveComponent> moveComponent;
	std::shared_ptr<CameraComponent> cameraComponent;

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

