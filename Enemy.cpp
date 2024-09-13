#include "Enemy.h"

#include <corecrt_math_defines.h>

#include "CollisionConfig.h"
#include "CollisionTypeIdDef.h"
#include "EnemyStatusConfig.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include "ViewProjection.h"

uint32_t Enemy::nextSerialNumber = 0;


Enemy::Enemy()
{
	//シリアルナンバーを振る
	serialNumber = nextSerialNumber;
	//次の番号を加算
	nextSerialNumber++;
}

void Enemy::Initialize(const std::vector<Model*>& models)
{
	/*---------------------[ワールド変換データの初期化]-----------------------*/
	//Nullポインタチェック
	//for (Model* model : models)
	//{
	//	assert(model);
	//}

	models_ = models;

	//ワールド変換初期化
	InitializeWorldTransform();

	//ステータスの初期化
	InitializeStatus();

	//バリアのα値を低くして透けさせる
	barrierColor_.Initialize();
	barrierColorValue_.w = 0.4f;

	//色変更して転送
	barrierColor_.SetColor(barrierColorValue_);
	barrierColor_.TransferMatrix();

	/*---------------------[種別IDの設定]-----------------------*/

	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));

}

void Enemy::Update()
{
	//生存フラグがfalseの場合は何もしない
	if(!isAlive_)
	{
		return;
	}

	//ImGui::Begin("Enemy");
	//float hp = GetHP();
	//ImGui::Text("HP %f",hp, 0.0f, 100.0f);
	//ImGui::SliderFloat3("Base Translation", &worldTransform_.translation_.x, -20.0f, 20.0f);
	//ImGui::SliderFloat3("ArmL Translation",&worldTransformL_arm_.translation_.x,-20.0f,20.0f);
	//ImGui::SliderFloat3("ArmR Translation",&worldTransformR_arm_.translation_.x,-20.0f,20.0f);
	//ImGui::NewLine();
	//ImGui::SliderFloat("Move Speed", &moveSpeed.x, 0.0f, 3.0f);
	//ImGui::End();

	Move();
	UpdateUI();
	UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection)
{
	//生存フラグがfalseの場合は何もしない
	if(!isAlive_)
	{
		return;
	}

	models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection);
	if(!canAttack_)
	{
		models_[kModelIndexBarrier]->Draw(worldTransformBarrier_, viewProjection,&barrierColor_);
	}

	if(GetHP() > 0)
	{
		models_[kModelIndexHPBar]->Draw(worldTransformHPBar_, viewProjection);
		models_[kModelIndexHPBarBack]->Draw(worldTransformHPBarBack_, viewProjection);
	}

}

void Enemy::Move()
{
	//向いている方向を変える
	worldTransform_.rotation_.y += 0.02f;

	//向いている方向に移動
	Vector3 velocity = moveSpeed;
	velocity = TransformNormal(velocity,worldTransform_.matWorld_);
	worldTransform_.translation_ += velocity;

}

void Enemy::UpdateMatrix()
{
	worldTransform_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
	worldTransformBarrier_.UpdateMatrix();
	worldTransformHPBar_.UpdateMatrix();
	worldTransformHPBarBack_.UpdateMatrix();
}

void Enemy::UpdateUI()
{
	/*------------------[ HPバーの向きをプレイヤーに向ける ]-------------------------*/

// ロックオン座標
	Vector3 playerPosition = player_->GetCenterPosition();
	// 追従対象からロックオン対象へのベクトル
	Vector3 sub = playerPosition - worldTransform_.translation_;
	// Y軸周り角度
	worldTransform_.rotation_.y = float(std::atan2(sub.x, sub.z) + M_PI); // 180度（πラジアン）を加える

	/*------------------[ HPの変動 ]-------------------------*/
	// 初期のスケールを設定
	Vector3 initialScale = { 1.0f, 1.0f, 1.0f };

	// HPに応じてスプライトのスケールを変更
	float hpRatio = static_cast<float>(GetHP()) / kInitialHp;
	float newScaleX = initialScale.x * hpRatio; // 初期スケールにHPの割合を掛ける
	// 新しいスケールを設定
	worldTransformHPBar_.scale_ = { newScaleX, initialScale.y, initialScale.z };



}

Vector3 Enemy::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

Vector3 Enemy::GetCenterCoordinate() const
{
	//見た目上の中心オフセット
	const Vector3 offset = { 0.0f, 1.0f, 0.0f };
	//ワールド座標に変換
	Vector3 worldPos = Transform(offset, worldTransform_.matWorld_);
	return worldPos;
}

Vector3 Enemy::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 1.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = Transform(offset, worldTransform_.matWorld_);
	return worldPos;
}

void Enemy::InitializeWorldTransform()
{
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 0.0f, 0.0f, 100.0f };


	//パーツも
	worldTransformBody_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.translation_ = { 2.6f, 0.0f, 0.0f };
	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.translation_ = { -2.6f, 0.0f, 0.0f };
	worldTransformBarrier_.Initialize();
	worldTransformBarrier_.translation_ = { 0.0f, 2.0f, 0.0f };
	worldTransformBarrier_.scale_ = { 4.0f,2.0f,4.0f };
	worldTransformHPBar_.Initialize();
	worldTransformHPBarBack_.Initialize();
	worldTransformHPBarBack_.translation_ = { 0.0f,3.0f,0.0f };

	//親子関係を結ぶ
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
	worldTransformBarrier_.parent_ = &worldTransformBody_;
	worldTransformHPBarBack_.parent_ = &worldTransformBody_;
	worldTransformHPBar_.parent_ = &worldTransformHPBarBack_;
}

void Enemy::OnCollision(Collider* other)
{
	//攻撃可能でなければ早期リターン
	if (!canAttack_) 
	{
		return;
	}

	//衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手がプレイヤー以外なら消す
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
	{
		//ダメージを与える
		other->TakeDamage(GetAttackPower());
	}

	if(GetHP() <= 0)
	{
		isAlive_ = false;
	}
}

void Enemy::InitializeStatus()
{
	//生存フラグ
	isAlive_ = true;
	
	//HP
	SetHP(kInitialHp);

	//速さ
	speed_ = kInitialSpeed;
	moveSpeed = { speed_,0.0f,0.0f };

	//攻撃力
	SetAttackPower(kInitialAttackPower);
}
