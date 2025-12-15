#pragma once
#include "CollisionComponent.h"
#include "RenderContext.h"
#include <memory>
#include <vector>

// 衝突判定マネージャー
class CollisionManager
{
private:
	CollisionManager() {}
	~CollisionManager() {}

public:
	// インスタンス取得
	static CollisionManager& Instance()
	{
		static CollisionManager instance;
		return instance;
	}

	// コンポーネントの登録
	void Register(std::shared_ptr<CollisionComponent> collision);

	// コンポーネントの登録解除
	void Unregister(std::shared_ptr<CollisionComponent> collision);

	// すべての衝突判定を実行
	void Update();

	// GUI描画
	void DrawGUI(RenderContext& rc);

	// 特定のコンポーネントとの衝突をチェック
	std::vector<std::shared_ptr<CollisionComponent>> CheckCollison(
		std::shared_ptr<CollisionComponent> collision);

private:
	std::vector<std::shared_ptr<CollisionComponent>> collisions;
};
