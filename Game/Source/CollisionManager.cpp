#include "CollisionManager.h"
#include "Graphics.h"
#include "Camera.h"
#include <algorithm>


// コンポーネントの登録
void CollisionManager::Register(std::shared_ptr<CollisionComponent> collision)
{
	// 既に登録されていないかチェック
	auto it = std::find(collisions.begin(), collisions.end(), collision);
	if (it == collisions.end())
	{
		collisions.emplace_back(collision);
	}
}

// コンポーネントの登録解除
void CollisionManager::Unregister(std::shared_ptr<CollisionComponent> collision)
{
	// 既に登録されていないかチェック
	auto it = std::find(collisions.begin(), collisions.end(), collision);
	if (it != collisions.end())
	{
		collisions.erase(it);
	}
}

// すべての衝突判定を実行
void CollisionManager::Update()
{
	// 総当たりで衝突判定
	size_t size = collisions.size();
	for (size_t i = 0; i < size; ++i)
	{
		std::shared_ptr<CollisionComponent> collisionA = collisions[i];
		if (!collisionA->GetActor()) continue; // Actorが削除されていたらスキップ

		for (size_t j = i + 1; j < size; ++j)
		{
			std::shared_ptr<CollisionComponent> collisionB = collisions[j];
			if (!collisionB->GetActor()) continue;

			DirectX::XMFLOAT3 outPosition;

			// 衝突判定実行
			if (collisionA->Intersect(collisionB, outPosition))
			{
				// 衝突イベントを呼ぶ
				collisionA->OnCollision(collisionB);
				collisionB->OnCollision(collisionA);

				// 押し出し処理は両方とも「トリガーではない」時のみ実行
				if (!collisionA->IsTrigger() && !collisionB->IsTrigger())
				{
					// 衝突した場合、押し出し処理
					std::shared_ptr<Actor> actorB = collisionB->GetActor();
					actorB->SetPosition(outPosition);
				}
			}
		}
	}
}

// GUI描画
void CollisionManager::DrawGUI(RenderContext& rc)
{
	Graphics& graphics = Graphics::Instance();
	ShapeRenderer* shapeRenderer = graphics.GetShapeRenderer();
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	RenderState* renderState = Graphics::Instance().GetRenderState();

	Camera& camera = CameraManager::Instance().GetMainCamera();

	rc.deviceContext = dc;
	rc.renderState = renderState;
	rc.camera = &camera;


	for (std::shared_ptr<CollisionComponent>& collision : collisions)
	{
		std::shared_ptr<Actor> actor = collision->GetActor();
		if (!actor) continue;

		DirectX::XMFLOAT3 pos = actor->GetPosition();
		float radius = collision->GetRadius();
		float height = collision->GetHeight();

		// 衝突形状に応じて描画
		if (collision->GetCollisionType() == CollisionType::Cylinder)
		{
			// 円柱をワイヤーフレームで描画
			DirectX::XMFLOAT4 color = { 0.0f, 1.0f, 0.0f, 1.0f }; // 緑
			shapeRenderer->DrawCylinder(pos, radius, height, color);
		}
		else if (collision->GetCollisionType() == CollisionType::Sphere)
		{
			// 球を描画
			DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 0.0f, 1.0f }; // 黄色
			shapeRenderer->DrawSphere(pos, radius, color);
		}

		shapeRenderer->Render(dc, camera.GetView(), camera.GetProjection());
	}
}

// 特定のコンポーネントとの処突チェック
std::vector<std::shared_ptr<CollisionComponent>> CollisionManager::CheckCollison(
	std::shared_ptr<CollisionComponent> collision)
{
	std::vector<std::shared_ptr<CollisionComponent>> hitCollisions;

	for (std::shared_ptr<CollisionComponent>& other : collisions)
	{
		if (collision == other) continue; // 自分自身は除外
		if (!other->GetActor()) continue;

		DirectX::XMFLOAT3 outPosition;
		if (collision->Intersect(other, outPosition))
		{
			hitCollisions.emplace_back(other);
		}
	}

	return hitCollisions;
}