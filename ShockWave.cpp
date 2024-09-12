#include "ShockWave.h"

#include <assert.h>

#include "CollisionTypeIdDef.h"
#include "MyMath.h"
#include "ShockWaveConfig.h"

void ShockWave::Initialize(Model* model, Vector3 position, Vector3 velocity,const ViewProjection* viewProjection)
{
	//Nullチェック
	assert(model);

	//引数で受け取ってメンバ変数に記録する
	model_ = model;
	velocity_ = velocity;

	viewProjection_ = viewProjection;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	isActive_ = true;

	SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kShockWave));

	//ステータスの初期化
	SetAttackPower(kShockWaveDamage);
	moveSpeed_ = kShockWaveMoveSpeed;

}

void ShockWave::Update()
{
	//有効でない場合は何もしない
	if (!isActive_)
	{
		return;
	}

	//移動方向を正規化
	Vector3 move = Normalize(velocity_);
	//移動速度を設定する
	move = Multiply(moveSpeed_, move);

	//移動
	worldTransform_.translation_ += move;
	//移動した距離を記録
	distanceTraveled_ += Length(move);

	//ワールド行列の更新
	worldTransform_.UpdateMatrix();

	if(distanceTraveled_ >= kMaxRange_)
	{
		isActive_ = false;
	}

}

void ShockWave::Draw(const ViewProjection& viewProjection)
{
	//有効でない場合は何もしない
	if(!isActive_)
	{
		return;
	}

	model_->Draw(worldTransform_, viewProjection);
}

void ShockWave::OnCollision([[maybe_unused]] Collider* other)
{
	//衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手がプレイヤー以外なら消す
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy))
	{
		//ダメージを受ける
		isActive_ = false;
	}
	
}

Vector3 ShockWave::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 1.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = Transform(offset, worldTransform_.matWorld_);
	return worldPos;
}

Vector3 ShockWave::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
