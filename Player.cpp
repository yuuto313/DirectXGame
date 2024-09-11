#include "Player.h"
#include "ViewProjection.h"
#include <assert.h>
#include "MyMath.h"
#include <Input.h>
#include <cmath>
#include <numbers>

#include "CollisionConfig.h"
#include "Easing.h"
#include "LockOn.h"

#include "ImGuiManager.h"

void Player::Initialize(const std::vector<Model*>& models,ViewProjection* viewProjection)
{
	//Nullポインタチェック
	for (Model* model : models)
	{
		assert(model);
	}

	//引数で受け取ったものを取得
	models_ = models;

	viewProjection_ = viewProjection;

	//初期化
	worldTransform_.Initialize();
	worldTransformBody_.Initialize();

	worldTransformHead_.Initialize();
	worldTransformHead_.translation_ = {0.0f, 4.0f, 0.0f};

	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.translation_ = {-0.625f, 3.75f, 0.0f};

	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.translation_ = {0.625f, 3.75f, 0.0f};

	//パーツ同士の親子関係
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	//ハンマーの初期化
	hammer_ = std::make_unique<Hammer>();
	std::vector<Model*> hammerModels = {models_[kModelindexWeapon], models_[kModelIndexEffect]};
	hammer_->Initialize(hammerModels, this);
	hammer_->SetParent(worldTransformBody_);

	InitializeFloatingGimick();

	behavior_ = Behavior::kRoot;

	//周期
	floatingCycle_ = 30;
	AttackCycle_ = 30;

	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeEnemy);

	UpdateMatrix();
}

void Player::Update()
{
	UpdateImGui();

	if (behaviorRequest_)
	{
		//振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		//各振る舞いごとの初期化を実行
		switch (behavior_)
		{
		case Behavior::kRoot:
		default :
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kJump:
			BehaviorJumpInitialize();
			break;
		}
		//振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_)
	{
		//通常行動
	case Behavior::kRoot:
	default :
		BehaviorRootUpdate();
		break;
		//攻撃行動
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	}

	hammer_->Update();

}

void Player::Draw()
{
	models_[kModelIndexBody]->Draw(worldTransformBody_, *viewProjection_);
	models_[kModelIndexHead]->Draw(worldTransformHead_, *viewProjection_);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, *viewProjection_);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, *viewProjection_);
	hammer_->Draw(*viewProjection_);
	hammer_->DrawEffect(*viewProjection_);
}


void Player::Move()
{
	XINPUT_STATE joyState;

	//コントローラが接続されているのなら
	if (Input::GetInstance()->GetJoystickState(0,joyState))
	{
		//速さ
		// float speed = 0.3f;

		//移動量
		velocity_ = {
			(float)joyState.Gamepad.sThumbLX,
			0.0f,
			(float)joyState.Gamepad.sThumbLY
		};

		//移動量に速さを反映
		//0除算にならないようにする
		if (Length(velocity_) > 0.0f)
		{
			velocity_ = Multiply(speed, Normalize(velocity_));
		}

		//移動ベクトルをカメラの角度だけ回転させる　
		velocity_ = Transform(velocity_, MakeRotateYMatrix(viewProjection_->rotation_.y)); 

		//移動
		worldTransform_.translation_ += velocity_;
	}

}

void Player::UpdateMatrix()
{
	worldTransform_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

}


void Player::Turn()
{
	//Y軸周り角度(0ｙ)
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	float velocityX = Length({ velocity_.x,0.0f,0.0f });
	//X軸周り角度(0x)
	worldTransform_.rotation_.x = std::atan2(0.0f, velocityX);

}

void Player::Attack()
{
	if (AttackParameter_ < 1) {
		// １フレーム分
		float step = 1.0f / AttackCycle_;

		// フレームを加算
		AttackParameter_ += step;

		// 腕の角度の始点と終点
		float armOriginRotation = std::numbers::pi_v<float>;                           // １８０度
		float armEndRotation = abs(armOriginRotation - 2 * std::numbers::pi_v<float> / 3); // ２７０度

		// 腕の回転
		worldTransformL_arm_.rotation_.x = armOriginRotation + EaseOutQuint<float>(AttackParameter_) * armEndRotation;
		worldTransformR_arm_.rotation_.x = armOriginRotation + EaseOutQuint<float>(AttackParameter_) * armEndRotation;
	}
}

void Player::TurnToTarget()
{
	//ロックオン座標
	Vector3 lockOnPosition = lockOn_->GetTargetPosition();
	//追従対象からロックオン対象へのベクトル
	Vector3 sub = lockOnPosition - worldTransform_.translation_;
	//Y軸周り角度
	worldTransform_.rotation_.y = std::atan2(sub.x, sub.z);
}

void Player::UpdateImGui() {
	ImGui::Begin("Floating Model");
	ImGui::SliderFloat3("Base Translation", &worldTransform_.translation_.x, -20.0f, 20.0f);
	ImGui::SliderFloat3("Head Translation", &worldTransformHead_.translation_.x, -20.0f, 20.0f);
	ImGui::SliderFloat3("ArmL Translation", &worldTransformL_arm_.translation_.x, -20.0f, 20.0f);
	ImGui::SliderAngle("ArmL Rotation", &worldTransformL_arm_.rotation_.x);
	ImGui::SliderFloat3("ArmR Translation", &worldTransformR_arm_.translation_.x, -20.0f, 20.0f);
	ImGui::SliderAngle("ArmR Rotation", &worldTransformR_arm_.rotation_.x);
	int cycle = static_cast<int>(floatingCycle_);
	ImGui::SliderInt("Cycle", &cycle, 0, 600);
	floatingCycle_ = static_cast<uint16_t>(cycle);
	ImGui::SliderFloat("Amplitude", &floatingAmplitude_, 0.0f, 10.0f);
	int attackCycle = static_cast<int>(AttackCycle_);
	ImGui::SliderInt("Attack Cycle", &attackCycle, 0, 600);
	AttackCycle_ = static_cast<uint16_t>(attackCycle);


	bool attack = false;
	ImGui::Checkbox("attack", &attack);
	if (attack) {
		behaviorRequest_ = Behavior::kAttack;
	}

	ImGui::End();
}

void Player::InitializeFloatingGimick()
{
	floatingParameter_ = 0.0f;
	floatingAmplitude_ = 0.4f;
	
}

void Player::UpdateFloatingGimick()
{
	const float step = 2.0f * std::numbers::pi_v<float> / floatingCycle_;

	//パラメータを１ステップ分加算
	floatingParameter_ += step;

	//2πを超えたら０に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * std::numbers::pi_v<float>);

	//浮遊を座標に反映
	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * floatingAmplitude_;

	//手をぶらぶら揺らす
	worldTransformL_arm_.rotation_.x = std::cos(floatingParameter_) * floatingAmplitude_;
	worldTransformR_arm_.rotation_.x = std::cos(floatingParameter_) * floatingAmplitude_;

}

void Player::BehaviorRootInitialize()
{
	InitializeFloatingGimick();
	worldTransformR_arm_.rotation_ = {};
	worldTransformL_arm_.rotation_ = {};
}

void Player::BehaviorRootUpdate()
{
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	if ((float)joyState.Gamepad.sThumbLX != 0.0f || (float)joyState.Gamepad.sThumbLY != 0.0f)
	{
		Move();
		Turn();

	} else if (lockOn_ && lockOn_->ExistTarget())
	{
		TurnToTarget();
	}
	// ジャンプボタンを押したら
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
		// ジャンプリクエスト
		behaviorRequest_ = Behavior::kJump;
	}

	//攻撃ボタンを押したら
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
		behaviorRequest_ = Behavior::kAttack;
	}

	//自キャラの待機アニメーション
	UpdateFloatingGimick();

	//行列の更新と転送
	UpdateMatrix();
}

void Player::BehaviorAttackInitialize()
{
	AttackParameter_ = 0.0f;
	hammer_->AttackAnimaitonInitialize();
}


void Player::BehaviorAttackUpdate()
{
	//ロックオン中
	if (lockOn_ && lockOn_->ExistTarget())
	{
		//TurnToTarget();

		// ロックオン座標
		Vector3 lockOnPosition = lockOn_->GetTargetPosition();
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = lockOnPosition - worldTransform_.translation_;

		//距離
		float distance = Length(sub);
		//距離閾値
		const float threshold = 0.2f;

		//閾値より離れているときのみ
		if (distance > threshold)
		{
			//Y軸周り角度
			worldTransform_.rotation_.y = std::atan2(sub.x, sub.z);

			//閾値を超える速さなら修正する
			if (speed > distance - threshold)
			{

				speed = distance - threshold;
			}
		}

	}
	Move();
	Attack();
	UpdateMatrix();
}

void Player::BehaviorJumpInitialize()
{
	worldTransformBody_.translation_.y = 0.0f;
	worldTransformL_arm_.rotation_.x = 0.0f;
	worldTransformR_arm_.rotation_.x = 0.0f;

	//ジャンプ初速
	const float kJumpFirstSpeed = 1.0f;
	//ジャンプ初速を与える
	velocity_.y = kJumpFirstSpeed;

}

void Player::BehaviorJumpUpdate()
{
	//移動
	worldTransform_.translation_ += velocity_;
	//重力加速度
	const float kGravityAcceleration = 0.05f;
	//加速度ベクトル
	Vector3 accelerationVector = {0.0f, -kGravityAcceleration, 0.0f};
	//加速する
	velocity_ += accelerationVector;

	//着地
	if (worldTransform_.translation_.y <= 0.0f)
	{
		worldTransform_.translation_.y = 0.0f;
		//ジャンプ終了
		behaviorRequest_ = Behavior::kRoot;
	}

	UpdateMatrix();

}

void Player::OnCollision()
{
	// ジャンプをリクエスト
		behaviorRequest_ = Behavior::kJump;
}

Vector3 Player::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
