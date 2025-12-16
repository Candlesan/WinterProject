#include "WeaponConponent.h"
#include "Player.h"
#include "Enemy.h"
#include <imgui.h>


// 開始
void WeaponComponent::Start()
{
}

// 更新
void WeaponComponent::Update(float elapsedTime)
{
	UpdateWeaponTransform();
}

// GUI描画
void WeaponComponent::OnGUI()
{
	ImGui::Text("Weapon Component");
	ImGui::Separator();

	// 親アクター名
	std::shared_ptr<Actor> parent = parentActor.lock();
	if (parent)
	{
		ImGui::Text("Parent: %s", parent->GetName());
	}

	ImGui::Text("Attach Bone: %s", attachBoneName.c_str());

	if (ImGui::CollapsingHeader("Local Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat3("Position", &localPosition.x, 0.01f);

		DirectX::XMFLOAT3 rotDeg = {
			DirectX::XMConvertToDegrees(localRotation.x),
			DirectX::XMConvertToDegrees(localRotation.y),
			DirectX::XMConvertToDegrees(localRotation.z)
		};
		if (ImGui::DragFloat3("Rotation", &rotDeg.x, 1.0f))
		{
			localRotation.x = DirectX::XMConvertToRadians(rotDeg.x);
			localRotation.y = DirectX::XMConvertToRadians(rotDeg.y);
			localRotation.z = DirectX::XMConvertToRadians(rotDeg.z);
		}

		ImGui::DragFloat3("Scale", &localScale.x, 0.001f);
	}

	if (ImGui::CollapsingHeader("Hit Detection", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat3("Hit Offset", &hitLocalPosition.x, 0.1f);
		ImGui::DragFloat("Hit Radius", &weaponHitRadius, 0.1f, 0.1f, 10.0f);

		DirectX::XMFLOAT3 hitPos = GetWeaponHitPosition();
		ImGui::Text("Hit Position: %.2f, %.2f, %.2f", hitPos.x, hitPos.y, hitPos.z);
	}
}

DirectX::XMFLOAT3 WeaponComponent::GetWeaponHitPosition() const
{
	std::shared_ptr<Actor> actor = GetActor();

	// 武器のワールド行列
	DirectX::XMMATRIX WeaponWorld = DirectX::XMLoadFloat4x4(&actor->GetTransform());

	// オフセット
	DirectX::XMVECTOR Offset = DirectX::XMLoadFloat3(&weaponHitOffset);

	// 攻撃判定位置
	DirectX::XMVECTOR HitPos = DirectX::XMVector3Transform(Offset, WeaponWorld);

	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, HitPos);
	return result;
}

void WeaponComponent::UpdateWeaponTransform()
{
	// 親アクター(キャラ)を取得
	std::shared_ptr<Actor> parent = parentActor.lock();
	if (!parent) return;

	// 自分のアクター（武器）を取得
	std::shared_ptr<Actor> actor = GetActor(); // 武器の見た目
	if (!actor) return;

	std::shared_ptr<Actor> hit = HitActor.lock(); // 攻撃判定

	// 親モデルを取得
	std::shared_ptr<Model> parentModel = parent->GetModel();
	if (!parentModel) return;

	// アタッチ先のボーンを検索
	const Model::Node* attachNode = nullptr;
	for (const Model::Node& node : parentModel->GetNodes())
	{
		if (node.name == attachBoneName)
		{
			attachNode = &node;
			break;
		}
	}
	if (!attachNode) return;

	if (actor)
	{
		// 武器のローカル行列を計算
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(localScale.x, localScale.y, localScale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(localRotation.x, localRotation.y, localRotation.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(localPosition.x, localPosition.y, localPosition.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ボーンのグローバル行列
		DirectX::XMMATRIX BoneGlobal = DirectX::XMLoadFloat4x4(&attachNode->globalTransform);

		// キャラのワールド行列
		DirectX::XMMATRIX ParentWorld = DirectX::XMLoadFloat4x4(&parent->GetTransform());

		// 武器のワールド行列　
		DirectX::XMMATRIX WeaponWorld = LocalTransform * BoneGlobal * ParentWorld;

		// 武器Actorの座標を更新
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 rotation;
		DirectX::XMFLOAT3 scale;

		// 行列から位置・回転・スケールを抽出
		DirectX::XMVECTOR s, r, t;
		DirectX::XMMatrixDecompose(&s, &r, &t, WeaponWorld);

		DirectX::XMStoreFloat3(&position, t);
		DirectX::XMStoreFloat4(&rotation, r);
		DirectX::XMStoreFloat3(&scale, s);

		actor->SetPosition(position);
		actor->SetRotation(rotation);
		actor->SetScale(scale);
	}

	if (hit)
	{
		DirectX::XMMATRIX S2 = DirectX::XMMatrixScaling(1, 1, 1);
		DirectX::XMMATRIX R2 = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);
		DirectX::XMMATRIX T2 = DirectX::XMMatrixTranslation(hitLocalPosition.x, hitLocalPosition.y, hitLocalPosition.z);
		DirectX::XMMATRIX HitLocalTransform = S2 * R2 * T2;

		// ボーンのグローバル行列
		DirectX::XMMATRIX BoneGlobal = DirectX::XMLoadFloat4x4(&attachNode->globalTransform);

		// キャラのワールド行列
		DirectX::XMMATRIX ParentWorld = DirectX::XMLoadFloat4x4(&parent->GetTransform());

		DirectX::XMMATRIX HitWorld = HitLocalTransform * BoneGlobal * ParentWorld;

		// 武器Actorの座標を更新
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 rotation;
		DirectX::XMFLOAT3 scale;

		// 行列から位置・回転・スケールを抽出
		DirectX::XMVECTOR s, r, t;
		DirectX::XMMatrixDecompose(&s, &r, &t, HitWorld);

		DirectX::XMStoreFloat3(&position, t);
		DirectX::XMStoreFloat4(&rotation, r);
		DirectX::XMStoreFloat3(&scale, s);

		hit->SetPosition(position);
		hit->SetRotation(rotation);
		hit->SetScale(scale);
	}
}
