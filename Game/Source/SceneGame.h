#pragma once
#include "Scene.h"
#include "Stage.h"
#include "Camera.h"
#include "FreeCameraController.h"
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
	Camera								camera;
	FreeCameraController				cameraController;
	std::shared_ptr<Stage> stage;
};
