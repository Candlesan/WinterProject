#include "MoveComponent.h"
#include "Actor.h"
#include "Input.h"
#include <imgui.h>

// 更新処理
void MoveComponent::Update(float elapsedTime)
{
	UpdateVelocity(elapsedTime);
}

// GUI描画
void MoveComponent::OnGUI()
{
	// 重力設定
	ImGui::Text("Physics");
	ImGui::DragFloat("Gravity", &gravity, 0.5f, -100.0f, 0.0f, "%.1f");
	ImGui::Checkbox("Is Grounded", &isGround);
	ImGui::Separator();

	// 速度表示（読み取り専用）
	ImGui::Text("Velocity (Read Only)");
	ImGui::Text("X: %.2f", velocity.x);
	ImGui::Text("Y: %.2f", velocity.y);
	ImGui::Text("Z: %.2f", velocity.z);
	float velocityMagnitude = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	ImGui::Text("Speed (XZ): %.2f", velocityMagnitude);
	ImGui::Separator();

	// 移動設定
	ImGui::Text("Movement");
	ImGui::DragFloat("Move Speed", &moveSpeed, 0.1f, 0.0f, 20.0f, "%.1f");
	ImGui::DragFloat("Friction", &friction, 0.5f, 0.0f, 50.0f, "%.1f");
	ImGui::DragFloat("Acceleration", &acceleration, 1.0f, 0.0f, 200.0f, "%.1f");
	ImGui::DragFloat("Max Move Speed", &maxMoveSpeed, 0.1f, 0.0f, 20.0f, "%.1f");
	ImGui::DragFloat("Jump Speed", &jumpSpeed, 0.1f, 0.0f, 100.0f, "%.1f");
	ImGui::SliderFloat("Air Control", &airControl, 0.0f, 1.0f, "%.2f");
	ImGui::Separator();

	// 移動ベクトル表示
	ImGui::Text("Move Vector (Read Only)");
	ImGui::Text("X: %.3f", moveVecX);
	ImGui::Text("Z: %.3f", moveVecZ);
	float moveLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
	ImGui::Text("Length: %.3f", moveLength);
	ImGui::Separator();

	// 旋回設定
	ImGui::Text("Rotation");
	float turnSpeedDegrees = DirectX::XMConvertToDegrees(turnSpeed);
	if (ImGui::DragFloat("Turn Speed (deg/s)", &turnSpeedDegrees, 10.0f, 0.0f, 1800.0f, "%.0f"))
	{
		turnSpeed = DirectX::XMConvertToRadians(turnSpeedDegrees);
	}
	ImGui::Separator();



	// リセットボタン
	ImGui::Separator();
	if (ImGui::Button("Reset to Default"))
	{
		gravity = -30.0f;
		moveSpeed = 5.0f;
		jumpSpeed = 12.0f;
		friction = 15.0f;
		acceleration = 50.0f;
		maxMoveSpeed = 5.0f;
		airControl = 0.3f;
		turnSpeed = DirectX::XMConvertToRadians(720.0f);
		velocity = { 0, 0, 0 };
		moveVecX = 0.0f;
		moveVecZ = 0.0f;
	}
}

// 移動処理
void MoveComponent::Move(float elapsedTime, float vx, float vz)
{
	//移動方向ベクトルを設定
	moveVecX = vx;
	moveVecZ = vz;

	//最大速度設定
	maxMoveSpeed = moveSpeed;
}

//void MoveComponent::Turn(float elapsedTime, float vx, float vz, float speed)
//{
//	std::shared_ptr<Actor> actor = GetActor();
//	DirectX::XMFLOAT4 angle = actor->GetRotation();
//
//	//進行ベクトルがゼロベクトルの場合は処理をする必要なし
//	float length = sqrtf(vx * vx + vz * vz);
//	if (length < 0.001f) return;
//
//	// 進行方向ベクトル
//	DirectX::XMVECTOR moveDir = DirectX::XMVectorSet(vx, 0.0f, vz, 0.0f);
//	moveDir = DirectX::XMVector3Normalize(moveDir);
//
//	// クォータニオンの計算
//	// Y軸を中心に、moveDir方向を前方向とする回転行列を作成し、クォータニオンに変換
//	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//	DirectX::XMVECTOR targetRot = DirectX::XMQuaternionRotationMatrix(DirectX::XMMatrixLookToLH(DirectX::XMVectorZero(), moveDir, up));
//
//	// 現在のクォータニオンを取得
//	DirectX::XMFLOAT4 currentRotF = actor->GetRotation();
//	DirectX::XMVECTOR currentRot = DirectX::XMLoadFloat4(&currentRotF);
//
//	// 球状線形補間でスムーズに回転
//	float factor = speed * elapsedTime;
//
//	// factorが1.0をけないようにクランプ
//	if (factor > 1.0f) factor = 1.0f;
//
//	DirectX::XMVECTOR newRotation = DirectX::XMQuaternionSlerp(currentRot, targetRot, factor);
//
//	// Actorに新しい回転を適用
//	DirectX::XMFLOAT4 newRotationF;
//	DirectX::XMStoreFloat4(&newRotationF, newRotation);
//	actor->SetRotation(newRotationF);
//}

//旋回処理
void MoveComponent::Turn(const DirectX::XMFLOAT3& direction, float elapsedTime)
{
	std::shared_ptr<Actor> actor = GetActor();
	float speed = turnSpeed * elapsedTime;
	DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
	DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&actor->GetRotation());
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationQuaternion(Rotation);
	DirectX::XMVECTOR OneZ = DirectX::XMVectorSet(0, 0, 1, 0);
	DirectX::XMVECTOR Front = DirectX::XMVector3TransformNormal(OneZ, Transform);

	Direction = DirectX::XMVector3Normalize(Direction);
	DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Front, Direction);
	if (DirectX::XMVector3Equal(Axis, DirectX::XMVectorZero()))
	{
		return;
	}

	DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Front, Direction);

	float dot;
	DirectX::XMStoreFloat(&dot, Dot);
	speed = (std::min)(1.0f - dot, speed);

	DirectX::XMVECTOR Turn = DirectX::XMQuaternionRotationAxis(Axis, speed);
	Rotation = DirectX::XMQuaternionMultiply(Rotation, Turn);

	DirectX::XMFLOAT4 rotation;
	DirectX::XMStoreFloat4(&rotation, Rotation);
	actor->SetRotation(rotation);
}

// ジャンプ処理
void MoveComponent::Jump()
{
	velocity.y = jumpSpeed;
}

//速力処理更新
void MoveComponent::UpdateVelocity(float elapsedTime)
{
	//垂直速力更新処理
	UpdateVerticalVelocity(elapsedTime);

	//水平速力更新処理
	UpdateHorizontalVelocity(elapsedTime);

	//垂直移動更新処理
	UpdateVerticalMove(elapsedTime);

	//水平移動更新処理
	UpdateHorizontalMove(elapsedTime);
}

//垂直速力更新処理
void MoveComponent::UpdateVerticalVelocity(float elapsedTime)
{
	//重力処理
	velocity.y += gravity * elapsedTime;
}

//垂直移動更新処理
void MoveComponent::UpdateVerticalMove(float elapsedTime)
{
	std::shared_ptr<Actor> actor = GetActor();
	DirectX::XMFLOAT3 position = actor->GetPosition();

	//移動処理
	position.y += velocity.y * elapsedTime;

	//地面判定
	if (position.y < 0.0f)
	{
		position.y = 0.0f;
		velocity.y = 0.0f;

		//着地した
		if (!isGround)
		{
			OnLanding();
			isGround = true;
		}
		else
		{
			isGround = false;
		}
	}
	actor->SetPosition(position);
}

//水平速力更新処理
void MoveComponent::UpdateHorizontalVelocity(float elapsedTime)
{
	//XZ平面の速力を減速
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		//摩擦力
		float friction = this->friction * elapsedTime;

		//空中にいるときは摩擦力を減らす
		if (!isGround) friction *= airControl;

		//摩擦による横方向の減速処理
		if (length > friction)
		{
			//単位ベクトル化
			float vx = velocity.x / length;
			float vz = velocity.z / length;
			//速力を減らす
			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}
		//横方向の速力が摩擦力以下になってたので速力を無効化
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	//XZ平面の速力を加速する
	if (length <= maxMoveSpeed)
	{
		//移動ベクトルがゼロベクトルでないなら加速する
		float moveVeclength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVeclength > 0.0f)
		{
			//加速力
			float acceleration = this->acceleration * elapsedTime;

			//空中にいるときは加速力を減らす
			if (!isGround) acceleration *= airControl;


			//移動ベクトルによる加速処理
			velocity.x += acceleration * moveVecX;
			velocity.z += acceleration * moveVecZ;

			//最大速度制限
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				//速度を単位ベクトル化
				float vx = velocity.x / length;
				float vz = velocity.z / length;
				//速度設定
				velocity.x = vx * maxMoveSpeed;
				velocity.z = vz * maxMoveSpeed;
			}
		}
	}
	//移動ベクトルをリセット
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

//水平移動更新処理
void MoveComponent::UpdateHorizontalMove(float elapsedTime)
{
	std::shared_ptr<Actor> actor = GetActor();
	DirectX::XMFLOAT3 position = actor->GetPosition();

	//移動処理
	position.x += velocity.x * elapsedTime;
	position.z += velocity.z * elapsedTime;

	actor->SetPosition(position);
}