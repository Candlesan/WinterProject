#include "Player.h"
#include "Graphics.h"
#include "Camera.h"
#include <imgui.h>


// コンストラクタ
void Player::Start()
{
	moveComponent = GetActor()->GetComponent<MoveComponent>();
	cameraComponent = GetActor()->GetComponent<CameraComponent>();
	if (cameraComponent)
	{
		cameraComponent->SetTarget(GetActor()); // 自身のアクターを渡す
		cameraComponent->SetDistance(5.0f);
		cameraComponent->SetOffset({ 0, 1.8f, 0 });
	}
}

// 更新処理
void Player::Update(float elapsedTime)
{
	CharacterControl(elapsedTime);
}

// プレイヤーコントローラー
void Player::CharacterControl(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//旋回処理
	moveComponent->Turn(moveVec, elapsedTime);

	//移動処理
	moveComponent->Move(elapsedTime, moveVec.x, moveVec.z);

	if (gamePad.GetButtonDown() & GamePad::BTN_SPACE)
	{
		moveComponent->Jump();
	}
}


//スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = CameraManager::Instance().GetMainCamera();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//移動ベクトルはXZ平面に水平なベクトルにする

	//カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		//単位ベクトル化 
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	//カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		//単位ベクトル化 
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	//スティックの水平入力値をカメラ右方向に反映し、
	//スティックの垂直入力値をカメラ前方向に反映し、
	//進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}
