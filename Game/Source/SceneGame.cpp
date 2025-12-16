#include "SceneGame.h"
#include "Graphics.h"

#include "Camera.h"
#include "FreeCameraController.h"

// コンポーネント
#include "Actor.h"
#include "MoveComponent.h"
#include "CameraComponent.h"
#include "CollisionManager.h"
#include "Player.h"
#include "Enemy.h"
#include "WeaponConponent.h"


// 初期化
void SceneGame::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// ステージモデルの初期化
	{
		std::shared_ptr<Actor> actor = ActorManager::Instance().Create();
		actor->LoadModel(device, "Data/Model/Stage/ExampleStage.glb");
		actor->SetName("Stage");
		actor->SetPosition({ 0, 0, 0 });
	}
	// プレイヤーの初期化
	{
		std::shared_ptr<Actor> player = ActorManager::Instance().Create();
		player->LoadModel(device, "Data/Model/unitychan/unitychan.glb");
		player->SetName("Player");
		player->SetPosition({ 0, 0, 0 });
		player->SetRotation({ 0, 0, 0, 1 });
		player->SetScale({ 1, 1, 1 });
		player->AddComponent<Player>();
		player->AddComponent<MoveComponent>();
		player->AddComponent<CameraComponent>();

		// 円柱の衝突判定を設定
		std::shared_ptr<CollisionComponent> collision = player->AddComponent<CollisionComponent>();
		collision->SetCylinder(0.5f, 0.5);

		// 武器
		std::shared_ptr<Actor> weapon = ActorManager::Instance().Create();
		weapon->LoadModel(device, "Data/Model/Weapon/Sword.glb");
		weapon->SetName("PlayerWeapon");

		// 攻撃判定
		std::shared_ptr<Actor> weaponHit = ActorManager::Instance().Create();
		weaponHit->SetName("PlayerWeaponHit");
		std::shared_ptr<CollisionComponent> weaponCollision = weaponHit->AddComponent<CollisionComponent>();
		weaponCollision->SetSphere(0.5f);
		weaponCollision->SetTrigger(true);

		// 武器の見た目
		std::shared_ptr<WeaponComponent> weaponComp = weapon->AddComponent<WeaponComponent>();
		weaponComp->SetParentActor(player);
		weaponComp->SetAttachBoneName("Character1_RightHand");
		weaponComp->SetLocalScale({ 0.01f, 0.01f, 0.01f });
		weaponComp->SetHitActor(weaponHit);
	}
	// エネミー初期化
	{
		std::shared_ptr<Actor> actor = ActorManager::Instance().Create();
		actor->LoadModel(device, "Data/Model/RPG-Character/RPG-Character.glb");
		actor->SetName("Enemy");
		actor->SetPosition({ 0, 0, 10 });
		std::shared_ptr<Enemy> enemy = actor->AddComponent<Enemy>();
		actor->AddComponent<MoveComponent>();

		// 円柱の衝突判定を設定
		std::shared_ptr<CollisionComponent> collision = actor->AddComponent<CollisionComponent>();
		collision->SetCylinder(0.5f, 0.5);

	}
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// すべてのアクターを更新する
	ActorManager::Instance().Update(elapsedTime);

	// 衝突判定を一括更新
	CollisionManager::Instance().Update();

	// すべてのアクターのモデルのワールド行列を更新
	ActorManager::Instance().UpdateTransform();
}

// 描画処理
void SceneGame::Render()
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();
	PrimitiveRenderer* primitiveRenderer = Graphics::Instance().GetPrimitiveRenderer();
	ModelRenderer* modelRenderer = Graphics::Instance().GetModelRenderer();

	// レンダーステート設定
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));

	Camera& camera = CameraManager::Instance().GetMainCamera();

	// 描画コンテキスト設定
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &camera;

	// 3Dモデル描画
	{
		// すべてのアクター内のモデルを描画
		ActorManager::Instance().Render(camera.GetView(), camera.GetProjection());
		modelRenderer->Render(rc);
	}

	CollisionManager::Instance().DrawGUI(rc);
}

// GUI描画
void SceneGame::DrawGUI()
{
}