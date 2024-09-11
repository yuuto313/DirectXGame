#include "ShockWave.h"

#include <assert.h>

#include "MyMath.h"

void ShockWave::Initialize(Model* model, Vector3 velocity)
{
	//Nullチェック
	assert(model);

	//引数で受け取ってメンバ変数に記録する
	model_ = model;
	velocity_ = velocity;
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

	if(distanceTraveled_ >= kMaxRange_)
	{
		isActive_ = false;
	}

}

void ShockWave::OnCollision()
{
	isActive_ = false;
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
