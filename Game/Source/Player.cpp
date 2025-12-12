#include "Player.h"
#include "Graphics.h"
#include "Camera.h"
#include <imgui.h>


// コンストラクタ
void Player::Start()
{
	moveComponent = GetActor()->GetComponent<MoveComponent>();
	cameraComponent = GetActor()->GetComponent<CameraComponent>();
	if (cameraComponent)
	{
		cameraComponent->SetTarget(GetActor()); // 自身のアクターを渡す
		cameraComponent->SetDistance(5.0f);
		cameraComponent->SetOffset({ 0, 1.8f, 0 });
	}
	GetActor()->GetModel()->GetNodePoses(nodePoses);
	GetActor()->GetModel()->GetNodePoses(oldNodePoses);
	int a = 0;
}

// 更新処理
void Player::Update(float elapsedTime)
{
	CharacterControl(elapsedTime);
	UpdateAnimation(elapsedTime);
}

// アニメーション更新処理
void Player::UpdateAnimation(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	float moveLength = sqrtf(moveVec.x * moveVec.x + moveVec.y * moveVec.y + moveVec.z * moveVec.z);

	int newAnimationIndex = animationIndex;

	switch (state)
	{
	case Player::State::Idle:
		animationLoop = true;
		useRootMotion = false;
		newAnimationIndex = GetActor()->GetModel()->GetAnimationIndex("Idle");

		if (moveLength > 0.01f)
		{
			state = State::Run;
		}

		break;
	case Player::State::Walk:
		break;
	case Player::State::Run:
		animationLoop = true;
		useRootMotion = false;
		newAnimationIndex = GetActor()->GetModel()->GetAnimationIndex("RunForwardInPlace");

		if (moveLength < 0.01f)
		{
			state = State::Idle;
		}

		break;
	case Player::State::Attack:
		break;
	}

	// アニメーション切り替え判定
	if (animationIndex != newAnimationIndex)
	{
		// 前のアニメーションの姿勢を保存
		oldNodePoses = nodePoses;

		// 新しいアニメーションに切り替え
		animationIndex = newAnimationIndex;
		animationSeconds = 0.0f;

		// 補間開始
		animationBlendSeconds = 0.0f;
		isBlending = true;

	}

	// 補間時間更新
	if (isBlending)
	{
		animationBlendSeconds += elapsedTime;
		if (animationBlendSeconds >= animationBlendSecondsLength)
		{
			isBlending = false;
			animationBlendSeconds = animationBlendSecondsLength;
		}
	}

	if (animationIndex >= 0)
	{
		const Model::Animation& animation = GetActor()->GetModel()->GetAnimations().at(animationIndex);

		// 現在のアニメーションの姿勢を取得
		std::vector<Model::NodePose> currentNodePoses;
		GetActor()->GetModel()->ComputeAnimation(animationIndex, animationSeconds, currentNodePoses);

		// 補間処理
		if (isBlending && animationBlendSeconds < animationBlendSecondsLength)
		{
			// 補間率を計算
			float blendRate = animationBlendSeconds / animationBlendSecondsLength;

			// 前のアニメーションと現在のアニメーションを補間
			GetActor()->GetModel()->BlendAnimations(oldNodePoses, currentNodePoses, blendRate, nodePoses);
		}
		else
		{
			// 補間なしで現在のアニメーション姿勢を使用
			nodePoses = currentNodePoses;
		}

		if (useRootMotion)
		{
			// ルートモーションノード番号取得
			const int rootMotionNodeIndex = GetActor()->GetModel()->GetNodeIndex("Character1_Hips");

			// 初回、前回、今回のルートモーションの姿勢を取得
			Model::NodePose beginPose, oldPose, newPose;
			GetActor()->GetModel()->ComputeAnimation(animationIndex, rootMotionNodeIndex, 0, beginPose);
			GetActor()->GetModel()->ComputeAnimation(animationIndex, rootMotionNodeIndex, oldAnimationSeconds, oldPose);
			GetActor()->GetModel()->ComputeAnimation(animationIndex, rootMotionNodeIndex, animationSeconds, newPose);

			//ローカル移動値を算出
			DirectX::XMFLOAT3 localTranslation;
			if (oldAnimationSeconds > animationSeconds)
			{
				// ループ時処理
				Model::NodePose endPose;
				GetActor()->GetModel()->ComputeAnimation(animationIndex, rootMotionNodeIndex, animation.secondsLength, endPose);

				// ローカル移動値を算出
				localTranslation.x = (endPose.position.x - oldPose.position.x) +
					(newPose.position.x - beginPose.position.x);
				localTranslation.y = (endPose.position.y - oldPose.position.y) +
					(newPose.position.y - beginPose.position.y);
				localTranslation.z = (endPose.position.z - oldPose.position.z) +
					(newPose.position.z - beginPose.position.z);
			}
			else
			{
				localTranslation.x = newPose.position.x - oldPose.position.x;
				localTranslation.y = newPose.position.y - oldPose.position.y;
				localTranslation.z = newPose.position.z - oldPose.position.z;
			}

			// グローバル移動値を算出
			Model::Node& rootMotionNode = GetActor()->GetModel()->GetNodes().at(rootMotionNodeIndex);
			DirectX::XMVECTOR LocalTranslation = DirectX::XMLoadFloat3(&localTranslation);
			DirectX::XMMATRIX ParentGlobalTransform = DirectX::XMLoadFloat4x4(&rootMotionNode.parent->globalTransform);
			DirectX::XMVECTOR GlobalTranslation = DirectX::XMVector3TransformNormal(LocalTranslation, ParentGlobalTransform);

			if (bakeTranslationY)
			{
				// Y成分の移動値を抜く 
				GlobalTranslation = DirectX::XMVectorSetY(GlobalTranslation, 0);

				// 今回の姿勢のグローバル位置を算出 
				DirectX::XMVECTOR LocalPos = DirectX::XMLoadFloat3(&newPose.position);
				DirectX::XMVECTOR currentGlobalPos = DirectX::XMVector3Transform(LocalPos, ParentGlobalTransform);

				// XZ成分を削除 
				currentGlobalPos = DirectX::XMVectorSetX(currentGlobalPos, 0);
				currentGlobalPos = DirectX::XMVectorSetZ(currentGlobalPos, 0);

				// ローカル空間変換 
				DirectX::XMMATRIX invGlobalTransform = DirectX::XMMatrixInverse(nullptr, ParentGlobalTransform);
				LocalPos = DirectX::XMVector3Transform(currentGlobalPos, invGlobalTransform);

				// ルートモーションノードの位置を設定 
				DirectX::XMStoreFloat3(&nodePoses[rootMotionNodeIndex].position, LocalPos);
			}
			else
			{
				//ルートモーションノードを初回の姿勢にする
				nodePoses[rootMotionNodeIndex].position = beginPose.position;
			}

			//ワールド移動値を算出
			DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);
			DirectX::XMVECTOR WorldTranslation = DirectX::XMVector3TransformNormal(GlobalTranslation, WorldTransform);
			DirectX::XMFLOAT3 worldTranslation;
			DirectX::XMStoreFloat3(&worldTranslation, WorldTranslation);

			//移動値を更新
			DirectX::XMFLOAT3 position = GetActor()->GetPosition();
			position.x += worldTranslation.x;
			position.y += worldTranslation.y;
			position.z += worldTranslation.z;
			GetActor()->SetPosition(position);
		}

		// アニメーション時間更新
		oldAnimationSeconds = animationSeconds;
		animationSeconds += elapsedTime;
		if (animationSeconds > animation.secondsLength)
		{
			if (animationLoop)
			{
				animationSeconds -= animation.secondsLength;
			}
			else
			{
				animationSeconds = animation.secondsLength;
			}
		}

		//姿勢更新
		GetActor()->GetModel()->SetNodePoses(nodePoses);
	}
}

// プレイヤーコントローラー
void Player::CharacterControl(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//旋回処理
	moveComponent->Turn(moveVec, elapsedTime);

	//移動処理
	moveComponent->Move(elapsedTime, moveVec.x, moveVec.z);

	if (gamePad.GetButtonDown() & GamePad::BTN_SPACE)
	{
		moveComponent->Jump();
	}
}

//スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = CameraManager::Instance().GetMainCamera();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//移動ベクトルはXZ平面に水平なベクトルにする

	//カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		//単位ベクトル化 
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		//単位ベクトル化 
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//スティックの水平入力値をカメラ右方向に反映し、
	//スティックの垂直入力値をカメラ前方向に反映し、
	//進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}
