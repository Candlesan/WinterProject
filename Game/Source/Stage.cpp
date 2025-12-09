#include "Stage.h"

// コンストラクタ
Stage::Stage(ID3D11Device* device)
{
	// ステージモデルを読み込み
	model = std::make_shared<Model>(device,"Data/Model/Stage/ExampleStage.glb");
}

// 更新処理
void Stage::Update(float elapsedTime)
{

}

// 描画処理
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	// レンダラにモデルを描画してもらう
	renderer->Draw(ShaderId::Lambert, model);
	renderer->Render(rc);
}
