#include "Enemy.h"
#include "Mathf.h"
#include "Player.h"
#include "BehaviorTree.h"
#include "BehaviorData.h"
#include "NodeBase.h"
#include "ActionDerived.h"
#include "JudgmentDerived.h"
#include "Graphics.h"
#include <imgui.h>

// 開始処理
void Enemy::Start()
{
	moveComponent = GetActor()->GetComponent<MoveComponent>();

	weaponActor = ActorManager::Instance().FindActorName("EnemyWeapon");
	if (weaponActor) {
		weaponCollision = weaponActor->GetComponent<WeaponCollision>();
	}

	GetActor()->SetRotationDegree(0, 180, 0);

	// ビヘイビアツリー設定
	//behaviorData = new BehaviorData();
	//aiTree = new BehaviorTree(this);

	// Root
	//aiTree->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

	// Battle
	//aiTree->AddNode("Root", "Battle", 2, BehaviorTree::SelectRule::Priority, new BattleJudgment(this), nullptr);
	//aiTree->AddNode("Battle", "Pursuit", 2, BehaviorTree::SelectRule::Non, nullptr, new PursuitAction(this));

	// Attack
	//aiTree->AddNode("Battle", "Attack", 1, BehaviorTree::SelectRule::Priority, new AttackJudgment(this), nullptr);
	//aiTree->AddNode("Attack", "Normal", 1, BehaviorTree::SelectRule::Non, new AttackJudgment(this), new AttackAction(this));

	// Scout
	//aiTree->AddNode("Root", "Scout", 3, BehaviorTree::SelectRule::Priority, nullptr, nullptr);
	//aiTree->AddNode("Scout", "Wander", 1, BehaviorTree::SelectRule::Non, new WanderJudgment(this), new WanderAction(this));
	//aiTree->AddNode("Scout", "Idle", 2, BehaviorTree::SelectRule::Non, nullptr, new IdleAction(this));

	// ModelとPlayerが循環参照してる可能性がある!
	GetActor()->GetModel()->GetNodePoses(nodePoses);
	GetActor()->GetModel()->GetNodePoses(oldNodePoses);

	state = State::Idle;
}

// 更新処理
void Enemy::Update(float elapsedTime)
{
	// 現在実行されているノードが無ければ
	//if (activeNode == nullptr)
	//{
	//	// 次に実行するノードを推論する。
	//	activeNode = aiTree->ActiveNodeInference(behaviorData);
	//}
	// 現在実行するノードがあれば
	//if (activeNode != nullptr)
	//{
	//	// ビヘイビアツリーからノードを実行。
	//	activeNode = aiTree->Run(activeNode, behaviorData, elapsedTime);
	//}

	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		state = State::Attack;
		auto weapon = GetWeaponCollision();
		if (weapon)
		{
			weapon->isActive = true;
		}
	}

	// 武器のデバッグ表示
	Graphics& graphics = Graphics::Instance();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	RenderDebugPrimitive(shapeRenderer);

	UpdateAnimation(elapsedTime);
}

// GUI描画
void Enemy::OnGUI()
{
	//std::string str = "";
	//if (activeNode != nullptr)
	//{
	//	str = activeNode->GetName();
	//}

	//ImGui::Text("Behavior %s", str.c_str());
	ImGui::Separator();

	// 武器調整用のGUIを追加
	if (ImGui::CollapsingHeader("Weapon Adjustment", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat3("Hit Offset", &weaponHitOffset.x, 0.01f);
		ImGui::DragFloat("Hit Radius", &weaponHitRadius, 0.01f, 0.1f, 5.0f);
	}

	if (ImGui::CollapsingHeader("Territory", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("TargetPosition : %f, %f, %f", targetPosition.x, targetPosition.y, targetPosition.z);
		ImGui::DragFloat3("TerritoryOrigin", &territoryOrigin.x, 0.01f);
		ImGui::DragFloat("TerritoryOrigin", &territoryRange, 0.01f);
	}
}

// デバックプリミティブ描画
void Enemy::RenderDebugPrimitive(ShapeRenderer* shapeRenderer)
{
	std::string rightHandName = "B_R_Hand";
	DirectX::XMFLOAT3 handWorldPos = { 0, 0, 0 };

	for (const Model::Node& node : GetActor()->GetModel()->GetNodes())
	{
		if (node.name == rightHandName)
		{
			DirectX::XMMATRIX handGlobal = DirectX::XMLoadFloat4x4(&node.globalTransform);
			DirectX::XMMATRIX enemyWorld = DirectX::XMLoadFloat4x4(&GetActor()->GetTransform());

			// ★オフセット行列を作成
			DirectX::XMMATRIX offsetT = DirectX::XMMatrixTranslation(weaponHitOffset.x, weaponHitOffset.y, weaponHitOffset.z);

			// ★ 手の行列 * オフセット * 親(敵)の行列
			DirectX::XMMATRIX finalHitMatrix = offsetT * handGlobal * enemyWorld;

			DirectX::XMVECTOR posVector = finalHitMatrix.r[3];
			DirectX::XMStoreFloat3(&handWorldPos, posVector);
			break;
		}
	}

	// 実際の武器アクター（当たり判定を持っている方）の位置を更新
	if (weaponActor) {
		weaponActor->SetPosition(handWorldPos);
		// ついでにコリジョンの半径も同期させるなら
		if (weaponCollision) {
			weaponCollision->SetSphere(weaponHitRadius);
		}
	}

	DirectX::XMFLOAT4 color = { 0.0f, 1.0f, 0.0f, 1.0f };
	shapeRenderer->DrawSphere(handWorldPos, weaponHitRadius, color);
}

std::shared_ptr<WeaponCollision> Enemy::GetWeaponCollision()
{
	// なければ探す（一度見つかれば次からはスルーされる）
	if (!weaponCollision) {
		if (!weaponActor) {
			weaponActor = ActorManager::Instance().FindActorName("EnemyWeapon");
		}
		if (weaponActor) {
			weaponCollision = weaponActor->GetComponent<WeaponCollision>();
		}
	}
	return std::static_pointer_cast<WeaponCollision>(weaponCollision);
}


// アニメーション更新
void Enemy::UpdateAnimation(float elapsedTime)
{
	int newAnimationIndex = animationIndex;

	switch (state)
	{
	case State::Idle:
		animationLoop = true;
		useRootMotion = false;
		newAnimationIndex = GetActor()->GetModel()->GetAnimationIndex("Idle");

		break;
	case State::Walk:
		animationLoop = true;
		useRootMotion = false;
		newAnimationIndex = GetActor()->GetModel()->GetAnimationIndex("Walk_F");

		break;
	case State::Run:
		break;
	case State::Attack:
		animationLoop = false;
		useRootMotion = false;
		newAnimationIndex = GetActor()->GetModel()->GetAnimationIndex("Slash");

		const Model::Animation& animation = GetActor()->GetModel()->GetAnimations().at(animationIndex);

		auto weapon = GetWeaponCollision();
		if (weapon)
		{
			if (animationSeconds >= 0.1f && animationSeconds <= 0.8f)
			{
				weapon->SetAttack(1, 0.5f);
				weapon->isActive = true;
			}
			else
			{
				weapon->isActive = false;
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
			const int rootMotionNodeIndex = GetActor()->GetModel()->GetNodeIndex("B_Pelvis");

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

// 縄張り設定 
void Enemy::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

// ターゲット位置をランダム設定
void Enemy::SetRandomTargetPosition()
{
	float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
	float range = Mathf::RandomRange(0.0f, territoryRange);
	targetPosition.x = territoryOrigin.x + sinf(theta) * range;
	targetPosition.y = territoryOrigin.y;
	targetPosition.z = territoryOrigin.z + cosf(theta) * range;
}

// 目標地点へ移動
void Enemy::MoveToTarget(float elapsedTime, float speedRate)
{
	std::shared_ptr<Actor> actor = GetActor();
	DirectX::XMFLOAT3 position = actor->GetPosition();

	// ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	// 移動処理
	moveComponent->Move(vx, vz, moveSpeed * speedRate);
	moveComponent->Turn({vx, 0.0f, vz}, elapsedTime);
}

// プレイヤー索敵
bool Enemy::SearchPlayer()
{
	// プレイヤーとの高低差を考慮して3Dで距離判定をする
	std::shared_ptr<Actor> actor = GetActor();
	DirectX::XMFLOAT3 EnemyPosition = actor->GetPosition();

	std::shared_ptr<Actor> playerActor = ActorManager::Instance().FindActorName("Player");
	if (!playerActor) return false;

	// プレイヤーの位置
	DirectX::XMFLOAT3 playerPos = playerActor->GetPosition();

	float vx = playerPos.x - EnemyPosition.x;
	float vy = playerPos.y - EnemyPosition.y;
	float vz = playerPos.z - EnemyPosition.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);

		// 単位ベクトル化
		vx /= distXZ;
		vz /= distXZ;

		DirectX::XMFLOAT4 rotation = actor->GetRotation();

		// クォータニオンから前方向ベクトルを計算
		DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&rotation);
		DirectX::XMVECTOR Forward = DirectX::XMVectorSet(0, 0, 1, 0);
		Forward = DirectX::XMVector3Rotate(Forward, Rotation);

		DirectX::XMFLOAT3 forward;
		DirectX::XMStoreFloat3(&forward, Forward);

		// 2つのベクトルの内積値で前後判定
		float dot = (forward.x * vx) + (forward.z * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}
	return false;
}

// アニメーションが終了したか
bool Enemy::FinshedAnimation()
{
	const Model::Animation& animation = GetActor()->GetModel()->GetAnimations().at(animationIndex);

	if (animationSeconds >= animation.secondsLength)
	{
		return true;
	}
	return false;
}
