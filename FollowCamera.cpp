#include "FollowCamera.h"

#include "ImGuiManager.h"
#include "Input.h"
#include "MyMath.h"
#include "LockOn.h"

void FollowCamera::Initialize()
{
	viewProjection_.Initialize();
}

void FollowCamera::Update()
{
	//追従
	Follow();
}

void FollowCamera::Follow()
{
	ImGui::Begin("Camera");
	ImGui::SliderFloat3("rotation", &viewProjection_.rotation_.x, 0.0f, 6.28f);
	ImGui::End();

		//追従対象がいれば
	if (lockOn_ && lockOn_->ExistTarget())
	{
		// ロックオン座標
		Vector3 lockOnPosition = lockOn_->GetTargetPosition();
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = lockOnPosition - target_->translation_;

		// Y軸周り角度
		viewProjection_.rotation_.y = std::atan2(sub.x, sub.z);

	}else
	{
		Turn();
	}

	// 追従対象からカメラまでのオフセット
	Vector3 offset = {0.0f, 5.0f, -20.0f};

	// 回転行列を求める
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(viewProjection_.rotation_.x); // X軸の回転
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(viewProjection_.rotation_.y); // Y軸の回転
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(viewProjection_.rotation_.z); // Z軸の回転
	Matrix4x4 rotateMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	// オフセットをカメラの回転に合わせて回転させる
	offset = TransformNormal(offset, rotateMatrix);
	// 座標をコピーしてオフセット文ずらす
	viewProjection_.translation_ = target_->translation_ + offset;

	//ビュー行列の更新と転送
	viewProjection_.UpdateViewMatrix();
	viewProjection_.TransferMatrix();
}

void FollowCamera::Turn()
{
	//ジョイスティックの状態
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0,joyState))
	{
		const float rotationSpeed = 0.000001f;

		viewProjection_.rotation_.y += (float)joyState.Gamepad.sThumbRX * rotationSpeed;
	}
}
