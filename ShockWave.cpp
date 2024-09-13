#include "ShockWave.h"

#include <assert.h>

#include "Chain.h"
#include "CollisionTypeIdDef.h"
#include "Enemy.h"
#include "MyMath.h"
#include "ShockWaveConfig.h"

void ShockWave::Initialize(std::vector<Model*> models, Vector3 position, Vector3 velocity,const ViewProjection* viewProjection,float damage)
{
	//Nullチェック
	//for(auto& model : models) {
	//	assert(model);
	//}

	//引数で受け取ってメンバ変数に記録する
	models_ = models;
	velocity_ = velocity;

	viewProjection_ = viewProjection;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	isActive_ = true;

	SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kShockWave));

	//ステータスの初期化
	SetAttackPower(damage);
	moveSpeed_ = kShockWaveMoveSpeed;

}

void ShockWave::Update()
{
	if (isActive_) {
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

	for (auto& hitEffect : hitEffects_) {
		hitEffect->Update();
	}

	//生存時間が０以下のヒットエフェクトを削除
	hitEffects_.remove_if([](const std::unique_ptr<HitEffect>& hitEffect) {
		return hitEffect->GetLifeTime() <= 0;
	});

}

void ShockWave::Draw(const ViewProjection& viewProjection)
{
	if (isActive_) {
		models_[kModelIndexWave]->Draw(worldTransform_, viewProjection);
	}
	for(auto& hitEffect : hitEffects_) {
		hitEffect->Draw(viewProjection);
	}
}

void ShockWave::OnCollision([[maybe_unused]] Collider* other)
{
	//アクティブでないなら何もしない
	if(!isActive_)
	{
		return;
	}

	//衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {

		// 敵のポインタを取得
		Enemy* enemy = static_cast<Enemy*>(other);

		if (!enemy->IsAlive())
		{
			return;
		}

		// 敵の位置にエフェクトを発生
		auto newHitEffect = std::make_unique<HitEffect>();
		newHitEffect->Initialize(models_[kModelIndexEffect], enemy->GetWorldPosition(),Vector3(7.0f,7.0f,7.0f));
		hitEffects_.push_back(std::move(newHitEffect));

		//ダメージを与える
		other->TakeDamage(GetAttackPower());
		isActive_ = false;
	}

	//衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kChain)) {

		// 敵のポインタを取得
		Chain* chain = static_cast<Chain*>(other);
		
		if (!chain->IsAlive())
		{
			return;
		}

		// 敵の位置にエフェクトを発生
		auto newHitEffect = std::make_unique<HitEffect>();
		newHitEffect->Initialize(models_[kModelIndexEffect], chain->GetCenterPosition(),Vector3(7.0f,7.0f,7.0f));
		hitEffects_.push_back(std::move(newHitEffect));

		//ダメージを与える
		other->TakeDamage(GetAttackPower());
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

bool ShockWave::IsActive() const
{
	if(!hitEffects_.empty()){
		// ヒットエフェクトが存在している間はアクティブ
		for(auto& hitEffect : hitEffects_) {
			if (hitEffect->GetLifeTime() > 0) {
				return true;
			}
		}
	}
	return isActive_;
}
