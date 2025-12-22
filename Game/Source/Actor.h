#pragma once
#include <memory>
#include <vector>
#include <set>
#include <string>
#include <DirectXMath.h>
#include "Model.h"
#include "Camera.h"

// 前方宣言
class Component;

// アクター
class Actor : public std::enable_shared_from_this<Actor>
{
public:
	Actor() {};
	virtual ~Actor() {}

	// 開始処理
	virtual void Start();

	// 更新処理
	virtual void Update(float elapsedTime);

	// 行列の更新
	virtual void UpdateTransform();

	// GUI表示
	virtual void OnGUI();

	// 名前の設定
	void SetName(const char* name) { this->name = name; }

	// 名前の取得
	const char* GetName() const { return name.c_str(); }

	// 位置の設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	// 位置の取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	// 回転の設定
	void SetRotation(const DirectX::XMFLOAT4& rotation) { this->rotation = rotation; }

	// 回転の取得
	const DirectX::XMFLOAT4& GetRotation() const { return rotation; }

	// スケールの設定
	void SetScale(const DirectX::XMFLOAT3 scale) { this->scale = scale; }

	// スケールの取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	// 行列の取得
	const DirectX::XMFLOAT4X4& GetTransform() const { return transform; }
	void SetTransform(DirectX::XMFLOAT4X4 Transform) { transform = Transform; }

	// モデルの読み込み
	void LoadModel(ID3D11Device* device ,const char* filename);

	// モデルの取得
	std::shared_ptr<Model> GetModel() const { return model; }

	// 移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	// 当たり判定の半径取得
	float GetRadius() { return radius; }

	// 当たり判定の半径設定
	void SetRadius(float rad) { radius = rad; }

	// 攻撃中か
	void SetAttacking(bool attaking) { isAttacking = attaking; }
	bool IsAttacking() const { return isAttacking; }

	// コンポーネント追加
	template<class T, class... Args>
	std::shared_ptr<T> AddComponent(Args... args) // args...:任意の引数にするためのテンプレートパラメーターパック
	{
		std::shared_ptr<T> component = std::make_shared<T>(args...); // コンポーネントのインスタンスの生成と初期化
		component->SetActor(shared_from_this()); // アクターの共有ポインタを取得

		// アクターへの参照が設定されたコンポーネントを、アクターが内部に持つ持つリストに追加
		components.emplace_back(component);
		return component;
	}


	// コンポーネント取得
	template<class T>
	std::shared_ptr<T> GetComponent()
	{
		// 今見ている要素が指定した型Tであるかを捜索
		for (std::shared_ptr<Component>& component : components)
		{
			std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
			if (p == nullptr) continue;
			return p;
		}
		return nullptr;
	}
private:
	std::string name;
	DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT4 rotation = DirectX::XMFLOAT4(0, 0, 0, 1);
	DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1, 1, 1);
	DirectX::XMFLOAT4X4 transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};

	float radius = 0.0f;
	bool isAttacking = false;

	std::shared_ptr<Model> model;

	std::vector<std::shared_ptr<Component>> components;
};

// アクターマネージャー
class ActorManager
{
private:
	ActorManager() {}
	~ActorManager() {}
public:
	// インスタンス取得
	static ActorManager& Instance()
	{
		static ActorManager instance;
		return instance;
	}

	// 作成
	std::shared_ptr<Actor> Create();

	// 削除
	void Remove(std::shared_ptr<Actor> actor);

	// 更新
	void Update(float elapsedTime);

	// 行列更新
	void UpdateTransform();

	// 描画
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& prorjection);

	// 指定のアクターを検索
	std::shared_ptr<Actor> FindActorName(const char* name);
private:
	void DrawLister();
	void DrawDetail();
private:
	std::vector<std::shared_ptr<Actor>>		startActors; // 次のフレームで始めて実行されるべきアクターを保存する変数
	std::vector<std::shared_ptr<Actor>>		updateActors;
	std::set<std::shared_ptr<Actor>>		selectionActors;
	std::set<std::shared_ptr<Actor>>		removeActors;

	bool					hiddenLister = false;
	bool					hiddenDetail = false;
};