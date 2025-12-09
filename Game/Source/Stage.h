#pragma once
#include "ModelRenderer.h"
#include <memory>

// ステージ
class Stage
{
public:
	Stage(ID3D11Device* device);
	~Stage() {};

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

private:
	std::shared_ptr<Model> model;
};
