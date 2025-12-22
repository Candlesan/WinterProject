#include "SceneGame.h"
#include "Graphics.h"

#include "Camera.h"
#include "FreeCameraController.h"

// コンポーネント
#include "Actor.h"
#include "CameraComponent.h"
#include "Player.h"
#include "Enemy.h"
#include "MoveComponent.h"
#include "CollisionManager.h"
#include "WeaponCollision.h"
#include "HealthComponent.h"


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
		player->LoadModel(device, "Data/Model/unitychan/assassin.gltf");
		player->SetName("Player");
		player->SetPosition({ 0, 0, 0 });
		player->SetRotation({ 0, 0, 0, 1 });
		player->SetScale({ 0.01f, 0.01f, 0.01f });
		player->AddComponent<Player>();
		player->AddComponent<MoveComponent>();
		player->AddComponent<CameraComponent>();

		// 体力を設定
		std::shared_ptr<HealthComponent> health = player->AddComponent<HealthComponent>();
		health->SetHealth(5);
		health->SetMaxHealth(5);

		// 円柱の衝突判定を設定
		std::shared_ptr<CollisionComponent> collision = player->AddComponent<CollisionComponent>();
		collision->SetCylinder(0.5f, 0.5);

		std::shared_ptr<Actor> weponActor = ActorManager::Instance().Create();
		weponActor->SetName("Weapon");
		// 武器用アクターには専用のコリジョンをつける
		auto weaponCollistion = weponActor->AddComponent<WeaponCollision>();
		weaponCollistion->SetSphere(0.5f);
		weaponCollistion->SetTrigger(true);
	}
	// エネミー初期化
	{
		std::shared_ptr<Actor> actor = ActorManager::Instance().Create();
		actor->LoadModel(device, "Data/Model/RPG-Character/RPG-Character.glb");
		actor->SetName("Enemy");
		actor->SetPosition({ 0, 0, 10 });
		std::shared_ptr<Enemy> enemy = actor->AddComponent<Enemy>();
		actor->AddComponent<MoveComponent>();

		// 体力を設定
		std::shared_ptr<HealthComponent> health = actor->AddComponent<HealthComponent>();
		health->SetHealth(5);
		health->SetMaxHealth(5);

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

	ShapeRenderer* shapeRenderer = Graphics::Instance().GetShapeRenderer();
	shapeRenderer->Render(dc, camera.GetView(), camera.GetProjection());
}

// GUI描画
void SceneGame::DrawGUI()
{
}