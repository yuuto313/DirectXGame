#include "Player.h"
#include "ViewProjection.h"
#include <assert.h>
#include "MyMath.h"
#include <Input.h>
#include <cmath>
#include <numbers>
#include <random>

#include "CollisionConfig.h"
#include "CollisionTypeIdDef.h"
#include "Easing.h"
#include "LockOn.h"

#include "ImGuiManager.h"
#include "PlayerStatusConfig.h"
#include "TextureManager.h"

Player::~Player()
{
	delete playerSpriteUI_;
	delete playerSpriteHP_;
	delete playerSpriteMP_;
	delete playerSkillSpPU_;
	delete playerSkillSpPD_;
	delete playerSkillSpSU_;
	delete playerSkillSpSD_;
}

void Player::Initialize(const std::vector<Model*>& models)
{
	//Nullポインタチェック
	for (Model* model : models)
	{
		assert(model);
	}

	//引数で受け取ったものを取得
	models_ = models;

	//ワールド変換の初期化
	InitializeWorldTransform();

	//ステータスの初期化
	InitializeStatus();

	//UIの初期化
	InitializeUI();

	//ハンマーの初期化
	InitializeHammer();

	//振る舞いの初期化
	InitializeFloatingGimick();

	behavior_ = Behavior::kRoot;

	//周期
	floatingCycle_ = 30;
	AttackCycle_ = 30;

	/*---------------------[種別IDの設定]-----------------------*/

	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer));

	//ワールド変換データを調整したので更新
	UpdateMatrix();
}

void Player::Update()
{
	//生存フラグが降りていたら何もしない
	if (!isAlive_)
	{
		return;
	}

	UpdateImGui();

	if (behaviorRequest_)
	{
		//振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		//各振る舞いごとの初期化を実行
		switch (behavior_)
		{
		case Behavior::kRoot:
		default:
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
	default:
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
	UpdateShockWave();
	UpdateUI();
}

void Player::Draw()
{
	//生存フラグが降りていたら何もしない
	if (!isAlive_)
	{
		return;
	}

	models_[kModelIndexBody]->Draw(worldTransformBody_, *viewProjection_);
	models_[kModelIndexHead]->Draw(worldTransformHead_, *viewProjection_);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, *viewProjection_);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, *viewProjection_);
	models_[kModelIndexAttackRange]->Draw(worldTransformAttackRange_, *viewProjection_);
	hammer_->Draw(*viewProjection_);
	hammer_->DrawEffect(*viewProjection_);
	DrawShockWave();
}


void Player::Move()
{
	XINPUT_STATE joyState;

	//コントローラが接続されているのなら
	if (Input::GetInstance()->GetJoystickState(0, joyState))
	{
		//速さ
		// float speed_ = 0.3f;

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
			velocity_ = Multiply(speed_, Normalize(velocity_));
		}

		//移動ベクトルをカメラの角度だけ回転させる　
		velocity_ = Transform(velocity_, MakeRotateYMatrix(viewProjection_->rotation_.y));

		//移動
		worldTransform_.translation_ += velocity_;
	}

	//移動制限
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -400.0f, 400.0f);
	worldTransform_.translation_.z = std::clamp(worldTransform_.translation_.z, -400.0f, 400.0f);

}

void Player::UpdateMatrix()
{
	worldTransform_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
	worldTransformAttackRange_.UpdateMatrix();

}

void Player::InitializeWorldTransform()
{
	//初期化
	worldTransform_.Initialize();
	worldTransformBody_.Initialize();

	worldTransformHead_.Initialize();
	worldTransformHead_.translation_ = { 0.0f, 0.0f, 0.0f };

	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.translation_ = { 0.0f, 0.0f, 0.0f };

	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.translation_ = { 0.0f, 0.0f, 0.0f };

	worldTransformAttackRange_.Initialize();

	//パーツ同士の親子関係
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
	worldTransformAttackRange_.parent_ = &worldTransform_;
}

void Player::InitializeHammer()
{
	//ハンマーの初期化
	hammer_ = std::make_unique<Hammer>();
	std::vector<Model*> hammerModels = { models_[kModelindexWeapon], models_[kModelIndexEffect] };
	hammer_->Initialize(hammerModels, this, GetAttackPower());
	hammer_->SetParent(worldTransformBody_);
}

void Player::InitializeStatus()
{
	SetHP(kInitialHp);
	speed_ = kInitialSpeed;
	SetAttackPower(kInitialAttackPower);
	skillDuration_ = kInitialSkillDuration;
	skillTimer_ = 0.0f;
	emotionGauge_ = kInitialEmotionGauge;
	emotionGaugeCost_ = kInitialEmotionGaugeCost;
}

void Player::InitializeUI()
{
	//===================================================
	// Player UI
	//===================================================

	playerTexUI_ = TextureManager::Load("playerUI/playerUI.png");
	playerSpriteUI_ = Sprite::Create(playerTexUI_, {});

	playerTexHP_ = TextureManager::Load("playerUI/playerHP.png");
	playerSpriteHP_ = Sprite::Create(playerTexHP_, {});

	playerTexMP_ = TextureManager::Load("playerUI/playerMP.png");
	playerSpriteMP_ = Sprite::Create(playerTexMP_, {});

	//HPばーの初期サイズを記録しておく
	hpUiInitialize_ = playerSpriteHP_->GetSize();

	//感情ゲージの初期サイズを記録しておく
	emotionGaugeUiInitilalize_ = playerSpriteMP_->GetSize();

	//===================================================
	// Player Skill UI 
	//===================================================

	playerSkillTexPU_ = TextureManager::Load("playerUI/sPowerUP.png");
	playerSkillSpPU_ = Sprite::Create(playerSkillTexPU_, {});

	playerSkillTexPD_ = TextureManager::Load("playerUI/sPowerDOWN.png");
	playerSkillSpPD_ = Sprite::Create(playerSkillTexPD_, {});

	playerSkillTexSU_ = TextureManager::Load("playerUI/sSpeedUP.png");
	playerSkillSpSU_ = Sprite::Create(playerSkillTexSU_, {});

	playerSkillTexSD_ = TextureManager::Load("playerUI/sSpeedDOWN.png");
	playerSkillSpSD_ = Sprite::Create(playerSkillTexSD_, {});
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
		//float armOriginRotation = std::numbers::pi_v<float>;                           // １８０度
		//float armEndRotation = abs(armOriginRotation - 2 * std::numbers::pi_v<float> / 3); // ２７０度
		//
		//// 腕の回転
		//worldTransformL_arm_.rotation_.x = armOriginRotation + EaseOutQuint<float>(AttackParameter_) * armEndRotation;
		//worldTransformR_arm_.rotation_.x = armOriginRotation + EaseOutQuint<float>(AttackParameter_) * armEndRotation;


		float armOriginRotation = -std::numbers::pi_v<float> / 5;
		float armEndRotation = std::numbers::pi_v<float> / 2;

		float armForwardRotationX = std::numbers::pi_v<float> / 4;

		worldTransformL_arm_.rotation_.x = armForwardRotationX;
		worldTransformR_arm_.rotation_.x = armForwardRotationX;

		worldTransformL_arm_.rotation_.y = armOriginRotation + EaseOutQuint<float>(AttackParameter_) * armEndRotation;
		worldTransformR_arm_.rotation_.y = armOriginRotation + EaseOutQuint<float>(AttackParameter_) * armEndRotation;

	} else
	{
		behaviorRequest_ = Behavior::kRoot;
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

void Player::SkillInitialize()
{
	// スキルがアクティブなら何もしない
	if (skillActive_)
	{
		return;
	}

	// スキルのゲージを消費
	emotionGauge_ -= emotionGaugeCost_;
	if (emotionGauge_ < 0.0f)
	{
		emotionGauge_ = 0.0f;
		return;
	}

	// スキルの効果を初期化
	skillActive_ = true;
	// ランダムな効果を選択するための乱数生成器
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 1);

	// ランダムに0または1を生成
	int random = dis(gen);

	if (random == 0) {
		// 移動速度アップ
		speed_ *= 2.0f; // 1.2倍に増加
		currentEffect_ = EffectType::SpeedUp;
	} else {
		// 攻撃力アップ
		SetAttackPower(GetAttackPower() * 2.0f); // 1.5倍に増加
		hammer_->SetAttackPower(GetAttackPower());
		currentEffect_ = EffectType::AttackUp;
	}

	skillTimer_ = skillDuration_;
	skillActive_ = true;
	lastUpdateTime_ = std::chrono::steady_clock::now();
}

void Player::SkillUpdate()
{
	// スキルがアクティブじゃないなら何もしない
	if (!skillActive_)
	{
		return;
	}

	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsed = now - lastUpdateTime_;
	skillTimer_ -= elapsed.count();
	lastUpdateTime_ = now;

	if (skillTimer_ <= 0.0f) {
		// 効果の持続時間が終了したら元に戻す
		if (currentEffect_ == EffectType::SpeedUp) {
			speed_ = kInitialSpeed;
		} else if (currentEffect_ == EffectType::AttackUp) {
			SetAttackPower(kInitialAttackPower);
			hammer_->SetAttackPower(GetAttackPower());
		}
		skillActive_ = false;
		currentEffect_ = EffectType::None;
	}
}

void Player::UpdateUI()
{
	// HPに応じてスプライトの大きさを変更
	float hpRatio = static_cast<float>(GetHP()) / kInitialHp;
	float newWidth = hpUiInitialize_.x * hpRatio; // 初期幅にHPの割合を掛ける
	playerSpriteHP_->SetSize({ newWidth,hpUiInitialize_.y }); // 高さは固定

	// 感情ゲージに応じてスプライトの大きさを変更
	float emotionGaugeRatio = emotionGauge_ / kInitialEmotionGauge;
	float newWidthEmotionGauge = emotionGaugeUiInitilalize_.x * emotionGaugeRatio; // 初期幅に感情ゲージの割合を掛ける
	playerSpriteMP_->SetSize({ newWidthEmotionGauge,emotionGaugeUiInitilalize_.y }); // 高さは固定



}

void Player::DrawUI()
{
	playerSpriteUI_->Draw();
	playerSpriteHP_->Draw();
	playerSpriteMP_->Draw();
	//スキルのアクティブ状態に応じてスプライトの種類を変更
	if (skillActive_)
	{
		switch (currentEffect_)
		{
		case EffectType::SpeedUp:
			playerSkillSpSU_->Draw();
			break;
		case EffectType::AttackUp:
			playerSkillSpPU_->Draw();
			break;
		}
	}

}

void Player::UpdateImGui() {
	ImGui::Begin("Floating Model");
	float hp = GetHP();
	ImGui::Text("HP %f", hp, 0.0f, 100.0f);
	ImGui::Text("emotionGauge %f", emotionGauge_);
	ImGui::Text("AttackPower %f", GetAttackPower());
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

void Player::OnCollision(Collider* other)
{
	//衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手が敵ならダメージを受ける
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy))
	{
		//ダメージを受ける
		TakeDamage(other->GetAttackPower());
	}

	if (GetHP() <= 0) {
		isAlive_ = false;
	}

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
	XINPUT_STATE preJoyState;
	Input::GetInstance()->GetJoystickStatePrevious(0, preJoyState);

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

	//スキルボタンを押したら
	if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) && !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_X))
	{
		SkillInitialize();
	}

	//スキルの更新
	SkillUpdate();

	//自キャラの待機アニメーション
	UpdateFloatingGimick();

	//行列の更新と転送
	UpdateMatrix();
}

void Player::BehaviorAttackInitialize()
{
	AttackParameter_ = 0.0f;
	hammer_->AttackAnimaitonInitialize();
	//衝撃波を生成
	GenerateShockWave();

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
			if (speed_ > distance - threshold)
			{

				speed_ = distance - threshold;
			}
		}

	} else if (lockOn_ && lockOn_->ExistChain())
	{
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
			if (speed_ > distance - threshold)
			{

				speed_ = distance - threshold;
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
	Vector3 accelerationVector = { 0.0f, -kGravityAcceleration, 0.0f };
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

void Player::GenerateShockWave()
{
	std::vector<Model*> models = {
		models_[kModelIndexShockWave],
		models_[kModelIndexEffect],
	};

	//ロックオンされているならロックオン対象の方向に向かって衝撃波を生成
	if ((lockOn_ && lockOn_->ExistTarget()) || (lockOn_ && lockOn_->ExistChain()))
	{
		//ターゲットの座標
		Vector3 targetPosition = lockOn_->GetTargetPosition();
		//高さの調整
		targetPosition.y -= 4.0f;
		//ロックオンの方向ベクトルを取得
		Vector3 velocity = targetPosition - worldTransform_.translation_;

		//衝撃波を生成
		std::unique_ptr<ShockWave> shockWave = std::make_unique<ShockWave>();
		shockWave->Initialize(models, hammer_->GetWorldPosition(), velocity, viewProjection_, GetAttackPower());
		shockWaves_.push_back(std::move(shockWave));
		return;
	}

	//ロックオンされていないならプレイヤーの前方向に向かって衝撃波を生成
	Vector3 velocity = { 0.0f,0.0f,1.0f };
	velocity = TransformNormal(velocity, worldTransform_.matWorld_);
	std::unique_ptr<ShockWave> shockWave = std::make_unique<ShockWave>();
	shockWave->Initialize(models, hammer_->GetWorldPosition(), velocity, viewProjection_, GetAttackPower());
	shockWaves_.push_back(std::move(shockWave));
}

void Player::UpdateShockWave()
{
	//衝撃波を更新
	for (const std::unique_ptr<ShockWave>& shockwave : shockWaves_)
	{
		shockwave->Update();
	}

	//無効な衝撃波を削除
	shockWaves_.remove_if([](const std::unique_ptr<ShockWave>& shockwave) {
		return !shockwave->IsActive();
		});
}

void Player::DrawShockWave()
{
	for (const std::unique_ptr<ShockWave>& shockwave : shockWaves_)
	{
		shockwave->Draw(*viewProjection_);
	}
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

Vector3 Player::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 1.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = Transform(offset, worldTransform_.matWorld_);
	return worldPos;
}