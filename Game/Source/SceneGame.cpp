#include "SceneGame.h"
#include "Graphics.h"

// コンポーネント
#include "Actor.h"


// 初期化
void SceneGame::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// カメラ設定
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// 画角
		screenWidth / screenHeight,			// 画面アスペクト比
		0.1f,								// ニアクリップ
		1000.0f								// ファークリップ
	);
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10), //視点
		DirectX::XMFLOAT3(0, 0, 0), //注視点
		DirectX::XMFLOAT3(0, 1, 0) //上方向
	);
	cameraController.SyncCameraToController(camera);

	// ステージモデルの初期化
	{
		std::shared_ptr<Actor> actor = ActorManager::Instance().Create();
		actor->LoadModel(device, "Data/Model/Stage/ExampleStage.glb");
		actor->SetName("Stage");
		actor->SetPosition({ 0, 0, 0 });
	}
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// カメラ更新処理
	cameraController.Update();
	cameraController.SyncControllerToCamera(camera);

	// すべてのアクターを更新する
	ActorManager::Instance().Update(elapsedTime);

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
}

// GUI描画
void SceneGame::DrawGUI()
{

}