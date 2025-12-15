#include "CollisionComponent.h"
#include "CollisionManager.h"
#include <imgui.h>


// デストラクタ
CollisionComponent::~CollisionComponent()
{
	// CollisionManagerから自動解除
	//CollisionManager::Instance().Unregister(shared_from_this());
}

// 開始処理
void CollisionComponent::Start()
{
	// CollisonManagerに自動登録
	CollisionManager::Instance().Register(shared_from_this());
}

// 更新処理
void CollisionComponent::Update(float elapsedTime)
{
	// 衝突判定は通常CollisionManagerなどで一括管理するため、
	// ここでは特に何もしない
}

// GUI描画
void CollisionComponent::OnGUI()
{
	// 衝突形状の選択
	const char* types[] = { "Sqhere", "Cylinder" };
	int currentType = static_cast<int>(collisionType);
	if (ImGui::Combo("Type", &currentType, types, IM_ARRAYSIZE(types)))
	{
		collisionType = static_cast<CollisionType>(currentType);
	}

	// パラメーター調整
	ImGui::DragFloat("Radius", &radius, 0.1f, 0.1f, 100.0f);

	if (collisionType == CollisionType::Cylinder)
	{
		ImGui::DragFloat("Height", &height, 0.1f, 0.1f, 100.0f);
	}
}

// ワールド座標での位置取得
DirectX::XMFLOAT3 CollisionComponent::GetWorldPosition() const
{
	std::shared_ptr<Actor> actor = GetActor();
	if (actor)
	{
		return actor->GetPosition();
	}
	return DirectX::XMFLOAT3(0, 0, 0);
}

// 他のコンポーネントとの衝突判定
bool CollisionComponent::Intersect(std::shared_ptr<CollisionComponent> other, DirectX::XMFLOAT3& outPosition)
{
	if (!other) return false;

	DirectX::XMFLOAT3 posA = GetWorldPosition();
	DirectX::XMFLOAT3 posB = other->GetWorldPosition();

	// 両方とも円柱の場合
	if (collisionType == CollisionType::Cylinder && other->GetCollisionType() == CollisionType::Cylinder)
	{
		return IntersectCylinderVsCylinder(
			posA, radius, height,
			posB, other->GetRadius(), other->GetHeight(),
			outPosition);
	}
	// 両方とも球の場合
	else if (collisionType == CollisionType::Sphere && other->GetCollisionType() == CollisionType::Sphere)
	{
		return IntersectSphereVsSphere(
			posA, radius,
			posB, other->GetRadius(),
			outPosition);
	}
	// 自分が球で相手が円柱の場合
	else if (collisionType == CollisionType::Sphere && other->GetCollisionType() == CollisionType::Cylinder)
	{
		return IntersectSphereVsCylinder(
			posA, radius,
			posB, other->GetRadius(), other->GetHeight(),
			outPosition);
	}
	// 自分が円柱で相手が球の場合
	else if (collisionType == CollisionType::Cylinder && other->GetCollisionType() == CollisionType::Sphere)
	{
		return IntersectSphereVsCylinder(
			posB, other->GetRadius(),
			posA, radius, height,
			outPosition);
	}

	return false;
}

//球と球の交差判定
bool CollisionComponent::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA, float radiusA,
	const DirectX::XMFLOAT3& positionB, float radiusB,
	DirectX::XMFLOAT3& outPositionB)
{
	//A→Bの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
	DirectX::XMVECTOR LenfthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LenfthSq);

	//距離判定
	float range = (radiusA + radiusB) * (radiusA + radiusB);
	if (lengthSq > range)
	{
		return false;
	}

	//AがBを押し出す
	DirectX::XMVECTOR NormalizeVec = DirectX::XMVector3Normalize(Vec);
	DirectX::XMVECTOR pushback = DirectX::XMVectorScale(NormalizeVec, radiusA + radiusB);
	DirectX::XMVECTOR newPosB = DirectX::XMVectorAdd(PositionA, pushback);
	DirectX::XMStoreFloat3(&outPositionB, newPosB);
	return true;
}

// 円柱と円柱の交差判定
bool CollisionComponent::IntersectCylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA, float radiusA, float heightA,
	const DirectX::XMFLOAT3& positionB, float radiusB, float heightB,
	DirectX::XMFLOAT3& outPositionB)
{
	//Aの足元がBの頭より上なら当たっていない
	if (positionA.y > positionB.y + heightB)
	{
		return false;
	}
	//Aの頭がBの足元よりしたなら当たっていない
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}
	//XZ平面での範囲チェック
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}
	//単位ベクトル
	vx /= distXZ;
	vz /= distXZ;
	//AがBを押し出す
	outPositionB.x = positionA.x + (vx * range);
	outPositionB.y = positionB.y;
	outPositionB.z = positionA.z + (vz * range);

	return true;
}

// 球と円柱の交差判定
bool CollisionComponent::IntersectSphereVsCylinder(
	const DirectX::XMFLOAT3& spherePosition, float sphereRadius,
	const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight,
	DirectX::XMFLOAT3& outCylinderPosition)
{
	// 球の底が円柱の頭より上なら当たっていない
	if (spherePosition.y > cylinderPosition.y + cylinderHeight) return false;

	// 球の頭が円柱の底より下なら当たっていない
	if (spherePosition.y + sphereRadius < cylinderPosition.y) return false;

	//XZ平面での範囲チェック
	float vx = cylinderPosition.x - spherePosition.x;
	float vz = cylinderPosition.z - spherePosition.z;
	float range = sphereRadius + cylinderRadius;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}

	//単位ベクトル化
	vx /= distXZ;
	vz /= distXZ;

	//球が円柱を押し出す
	outCylinderPosition.x = spherePosition.x + (vx * range);
	outCylinderPosition.y = cylinderPosition.y;
	outCylinderPosition.z = spherePosition.z + (vz * range);

	return true;
}
