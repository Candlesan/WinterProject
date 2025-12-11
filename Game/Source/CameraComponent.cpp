#include "CameraComponent.h"
#include "imgui.h"
#include <DirectXMath.h>


// 開始
void CameraComponent::Start()
{
	Camera& camera = CameraManager::Instance().GetMainCamera();
	camera.SetPerspectiveFov(DirectX::XMConvertToRadians(45.0f), 16.0f / 9.0, 0.1f, 1000.0f);
}

// 更新
void CameraComponent::Update(float elapsedTime)
{
	if (!target) return;

	GamePad& gamePad = Input::Instance().GetGamePad();

	// 右スティックの入力を取得
	float ax = gamePad.GetAxisRX();
	float ay = gamePad.GetAxisRY();

	float lengthSq = ax * ax + ay * ay;
	if (lengthSq > 0.1f)
	{
		float speed = rollSpeed * elapsedTime;

		// ピッチとヨーの角度を更新
		angle.x += ay * speed;
		angle.y += ax * speed;

        // ピッチ角の制限
        if (angle.x < minPitch)
        {
            angle.x = minPitch;
        }
        if (angle.x > maxPitch)
        {
            angle.x = maxPitch;
        }
        // ヨー角の調整（-πからπの範囲に収める）
        if (angle.y < -DirectX::XM_PI)
        {
            angle.y += DirectX::XM_2PI;
        }
        if (angle.y > DirectX::XM_PI)
        {
            angle.y -= DirectX::XM_2PI;
        }
    }

    // カメラの始点と注視点を計算
    DirectX::XMVECTOR TargetPos = DirectX::XMLoadFloat3(&target->GetPosition());
    DirectX::XMVECTOR OffsetVec = DirectX::XMLoadFloat3(&offset);
    DirectX::XMVECTOR Focus = DirectX::XMVectorAdd(TargetPos, OffsetVec);

    DirectX::XMMATRIX Rot = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    DirectX::XMVECTOR Back = DirectX::XMVectorSet(0, 0, -distance, 0);
    DirectX::XMVECTOR Eye = DirectX::XMVectorAdd(Focus, DirectX::XMVector3TransformCoord(Back, Rot));

    DirectX::XMFLOAT3 eye, focus;
    DirectX::XMStoreFloat3(&eye, Eye);
    DirectX::XMStoreFloat3(&focus, Focus);

    // 計算した始点と注視点を使ってメインカメラを更新
    Camera& camera = CameraManager::Instance().GetMainCamera();
    camera.SetLookAt(eye, focus, DirectX::XMFLOAT3(0, 1, 0));
}

// GUI描画
void CameraComponent::OnGUI()
{
    ImGui::Text("CameraComponent Details:");
    ImGui::DragFloat3("Offset", &offset.x, 0.1f);
    ImGui::InputFloat("Distance", &distance, 0.1f);
    ImGui::InputFloat3("Angles (Pitch, Yaw, Roll)", &angle.x, "%.2f"); // カメラの現在の角度
    ImGui::InputFloat("Roll Speed", &rollSpeed, 0.01f);
    ImGui::InputFloat("Max Pitch", &maxPitch, 0.01f);
    ImGui::InputFloat("Min Pitch", &minPitch, 0.01f);
}
