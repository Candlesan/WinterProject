#pragma once
#include "Component.h"
#include <DirectXMath.h>

// 衝突形状の種類
enum class CollisionType
{
	Sphere, // 球
	Cylinder // 円柱
};

// 衝突判定コンポーネント
class CollisionComponent : public Component, public std::enable_shared_from_this<CollisionComponent>
{
public:
	CollisionComponent() {}
	~CollisionComponent();

	// 名前取得
	const char* GetName() const override { return "Collision"; }

	// 開始処理
	void Start() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// GUI描画
	void OnGUI() override;

	// 衝突形状の設定
	void SetCylinder(float radius, float height)
	{
		collisionType = CollisionType::Cylinder;
		this->radius = radius;
		this->radius = height;
	};

	void SetSphere(float radius)
	{
		collisionType = CollisionType::Cylinder;
		this->radius = radius;
		this->height = radius * 2.0f; // 球の高さは直径 : d = r * r
	};

	// 衝突形状の取得
	CollisionType GetCollisionType() const { return collisionType; }
	float GetRadius() const { return radius; }
	float GetHeight() const { return height; }

	// ワールド座標での位置取得
	DirectX::XMFLOAT3 GetWorldPosition() const;

	// 他のコンポーネントとの衝突判定
	bool Intersect(std::shared_ptr<CollisionComponent> other, DirectX::XMFLOAT3& outPosition);

	// 衝突時のコールバック（必要に応じてオーバーライド）
	virtual void  OnCollision(std::shared_ptr<CollisionComponent> other) {}

private:
	//球と球の交差判定
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA, float radiusA,
		const DirectX::XMFLOAT3& positionB, float radiusB,
		DirectX::XMFLOAT3& outPositionB
	);

	// 円柱と円柱の交差判定
	static bool IntersectCylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA, float radiusA, float heightA,
		const DirectX::XMFLOAT3& positionB, float radiusB, float heightB,
		DirectX::XMFLOAT3& outPositionB);

	// 球と円柱の交差判定
	static bool IntersectSphereVsCylinder(
		const DirectX::XMFLOAT3& spherePosition, float sphereRadius,
		const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition);

private:
	CollisionType collisionType = CollisionType::Cylinder;
	float radius = 0.5f;
	float height = 2.0f;
};
