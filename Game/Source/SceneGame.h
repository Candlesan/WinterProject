#pragma once
#include "Scene.h"
#include "Model.h"


// ゲームシーン
class SceneGame
{
public:
	SceneGame() {};
	~SceneGame() {};

	// 初期化
	void Initialize();

	// 終了化
	void Finalize() {};

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render();

	// GUI描画
	void DrawGUI();
private:
	float pbrMetalness = 0.0f;
	float pbrRoughness = 0.0f;
	DirectX::XMFLOAT3 lightDir = { 0.0f, -1.0f, 0.0f };
};
