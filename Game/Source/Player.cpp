#include "Player.h"
#include "Graphics.h"
#include "Camera.h"
#include <imgui.h>


// 開始処理
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

	weaponActor = ActorManager::Instance().FindActorName("Weapon");
	if (weaponActor) {
		weaponCollision = weaponActor->GetComponent<WeaponCollision>();
	}

	// 武器のモデル読み込み
	ID3D11Device* device = Graphics::Instance().GetDevice();
	weaponModel = std::make_shared<Model>(device, "Data/Model/Weapon/Sword.glb");

	// ModelとPlayerが循環参照してる可能性がある!
	GetActor()->GetModel()->GetNodePoses(nodePoses);
	GetActor()->GetModel()->GetNodePoses(oldNodePoses);
}

// 更新処理
void Player::Update(float elapsedTime)
{
	CharacterControl(elapsedTime);
	UpdateAnimation(elapsedTime);
	WeaponAttachment(elapsedTime);

	// 武器のデバッグ表示
	Graphics& graphics = Graphics::Instance();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	RenderDebugPrimitive(shapeRenderer);

	weaponModel->UpdateTransform(WeaponTransform);
}

// GUI描画
void Player::OnGUI()
{
	DirectX::XMFLOAT3 hitPos = GetWeaponHitPosition();
	// ImGuiで調整
	if (ImGui::CollapsingHeader("Weapon Debug", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat3("Weapon Position", &localPosition.x, 0.1f);
		ImGui::DragFloat3("Weapon Angle", &localAngle.x, 0.1f);
		ImGui::DragFloat3("Weapon Scale", &localScale.x, 0.1f);

		ImGui::Separator();
		ImGui::DragFloat3("Hit Offset", &weaponHitOffset.x, 0.1f);
		ImGui::DragFloat("Hit Radius", &weaponHitRadius, 0.1f, 0.1f, 10.0f);
		ImGui::Text("Hit Position: %.2f, %.2f, %.2f", hitPos.x, hitPos.y, hitPos.z);
	}
}

// デバックプリミティブ描画
void Player::RenderDebugPrimitive(ShapeRenderer* shapeRenderer)
{
	if (!weaponActor) {
		// もし NULL なら、今この瞬間に再度探してみる
		weaponActor = ActorManager::Instance().FindActorName("Weapon");
		if (!weaponActor) return; // それでもいなければ諦めて帰る
	}

	// 攻撃判定位置を取得
	DirectX::XMFLOAT3 hitPos = GetWeaponHitPosition();

	weaponActor->SetPosition(hitPos);

	DirectX::XMFLOAT4 color = { 1.0f, 0.0f, 0.0f, 1.0f };
	shapeRenderer->DrawSphere(hitPos, weaponHitRadius, color);
}

// アニメーション更新処理
void Player::UpdateAnimation(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	DirectX::XMFLOAT3 moveVec = GetActor()->GetMoveVec();
	float moveLength = sqrtf(moveVec.x * moveVec.x + moveVec.y * moveVec.y + moveVec.z * moveVec.z);

	int newAnimationIndex = animationIndex;


	switch (state)
	{
	case Player::State::Idle:
		animationLoop = true;
		useRootMotion = false;
		newAnimationIndex = GetActor()->GetModel()->GetAnimationIndex("idle");

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
		newAnimationIndex = GetActor()->GetModel()->GetAnimationIndex("run");

		if (moveLength < 0.01f)
		{
			state = State::Idle;
		}

		break;
	case Player::State::Attack:
		animationLoop = false;
		useRootMotion = false;
		newAnimationIndex = GetActor()->GetModel()->GetAnimationIndex("slash");

		const Model::Animation& animation = GetActor()->GetModel()->GetAnimations().at(animationIndex);		

		auto weapon = GetWeaponCollision();

		if (weapon)
		{
			if (animationSeconds >= 0.2f && animationSeconds <= 0.7f)
			{
				weaponCollision->SetAttack(3, 0.5f);
				weaponCollision->isActive = true;

				weaponActor->SetPosition(GetWeaponHitPosition());
			}
			else
			{
				weaponCollision->isActive = false;
			}
		}

		if (animationSeconds >= animation.secondsLength)
		{
			state = State::Idle;
		}

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

std::shared_ptr<WeaponCollision> Player::GetWeaponCollision()
{
	// なければ探す（一度見つかれば次からはスルーされる）
	if (!weaponCollision) {
		if (!weaponActor) {
			weaponActor = ActorManager::Instance().FindActorName("Weapon");
		}
		if (weaponActor) {
			weaponCollision = weaponActor->GetComponent<WeaponCollision>();
		}
	}
	return std::static_pointer_cast<WeaponCollision>(weaponCollision);
}

// 武器のアタッチメント
void Player::WeaponAttachment(float elapsedTime)
{
	std::string rightHandName = "mixamorig:RightHand";

	ModelRenderer* modelremderer = Graphics::Instance().GetModelRenderer();

	// 武器のローカル行列を計算する
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(localScale.x, localScale.y, localScale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(localAngle.x, localAngle.y, localAngle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(localPosition.x, localPosition.y, localPosition.z);
	DirectX::XMMATRIX weaponLocal = S * R * T;

	// キャラクターのモデルから右手ノードを検索する
	for (const Model::Node& node : GetActor()->GetModel()->GetNodes())
	{
		if (node.name == rightHandName)
		{
			// 右手ノードと武器のローカル行列から武器のワールド行列を求める
			DirectX::XMMATRIX rightHandGlobal = DirectX::XMLoadFloat4x4(&node.globalTransform);
			DirectX::XMMATRIX playerWorld = DirectX::XMLoadFloat4x4(&GetActor()->GetTransform());
			DirectX::XMMATRIX weaponWorld = weaponLocal * rightHandGlobal * playerWorld;

			DirectX::XMStoreFloat4x4(&WeaponTransform, weaponWorld);

			modelremderer->Draw(ShaderId::Lambert, weaponModel);
			break;
		}
	}
}

// 攻撃判定位置を取得
DirectX::XMFLOAT3 Player::GetWeaponHitPosition() const
{
	// 武器のワールド行列とオフセット
	DirectX::XMMATRIX WeaponWorld = DirectX::XMLoadFloat4x4(&WeaponTransform);
	DirectX::XMVECTOR Offset = DirectX::XMLoadFloat3(&weaponHitOffset);

	DirectX::XMVECTOR HitPos = DirectX::XMVector3Transform(Offset, WeaponWorld);

	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, HitPos);

	return result;
}

// プレイヤーコントローラー
void Player::CharacterControl(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	healthComponent = GetActor()->GetComponent<HealthComponent>();

	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetActor()->GetMoveVec();

	//旋回処理
	moveComponent->Turn(moveVec, elapsedTime);

	//移動処理
	moveComponent->Move(elapsedTime, moveVec.x, moveVec.z);

	// スペースキーを押すとジャンプ
	if (gamePad.GetButtonDown() & GamePad::BTN_SPACE)
	{
		moveComponent->Jump();
	}

	// Zキーを押すと攻撃
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		if (!GetActor()->IsAttacking())
		{
			state = State::Attack;
			auto weapon = GetWeaponCollision();
			if (weapon)
			{
				weaponCollision->isActive = true;
			}
		}
	}
}
