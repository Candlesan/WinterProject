#pragma once

#include "Actor.h"

// コンポーネント
class Component
{
public:
	Component() {}
	~Component() {}

	// 名前取得
	virtual const char* GetName() const = 0;

	// 開始処理
	virtual void Start() {}

	// 更新処理
	virtual void Update(float elpasedTime) {}

	// GUI描画
	virtual void OnGUI() {}

	// アクター設定
	void SetActor(std::shared_ptr<Actor> actor) { this->actor = actor; }

	// アクター取得
	std::shared_ptr<Actor> GetActor() const { return actor.lock(); }

private:
	std::weak_ptr<Actor> actor;
};