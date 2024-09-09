#include "Hammer.h"

#include <numbers>

#include "CollisionConfig.h"
#include "CollisionTypeIdDef.h"
#include "Easing.h"
#include "Enemy.h"
#include "MyMath.h"
#include "Player.h"

void Hammer::Initialize(std::vector<Model*> models,Player* player)
{
	//引数で受け取ってメンバ変数に記録する
	models_ = models;
	player_ = player;

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = { 0.0f, 4.0f, 0.0f };
	

	/*---------------------[種別IDの設定]-----------------------*/

	SetCollisionAttribute(kCollisionAttributeHammer);
	SetCollisionMask(kCollisionAttributeEnemy);
}

void Hammer::Update()
{
	/*---------------------[アニメーションの更新]-----------------------*/

	switch (player_->GetBehavior())
	{
		case Behavior::kAttack:
			AttackAnimationUpdate();
		    break;
	}

	/*---------------------[ヒットエフェクトの更新]-----------------------*/

	for (auto& hitEffect : hitEffects_) {
		hitEffect->Update();
	}

	//生存時間が０以下のヒットエフェクトを削除
	hitEffects_.remove_if([](const std::unique_ptr<HitEffect>& hitEffect) {
		return hitEffect->GetLifeTime() <= 0;
	});

	worldTransform_.UpdateMatrix();
	

}

void Hammer::Draw(const ViewProjection& viewProjection)
{
	if (player_->GetBehavior() != Behavior::kAttack) {
		return;
	}
	models_[kIndexModelHammer]->Draw(worldTransform_, viewProjection);
}

void Hammer::DrawEffect(const ViewProjection& viewProjection)
{
	for (auto& hitEffect : hitEffects_) {
		hitEffect->Draw(viewProjection);
	}
}

void Hammer::OnCollision()
{
}

void Hammer::OnCollision(Collider* other)
{
	// 敵のポインタを取得
	Enemy* enemy = static_cast<Enemy*>(other);
	uint32_t serialNumber = enemy->GetSerialNumber();

	// 敵の位置にエフェクトを発生
	auto newHitEffect = std::make_unique<HitEffect>();
	newHitEffect->Initialize(models_[kIndexModelEffect],enemy->GetWorldTransform().translation_);
	hitEffects_.push_back(std::move(newHitEffect));
	
}


void Hammer::AttackAnimaitonInitialize()
{
	AttackParameter_ = 0;
	AttackCycle_ = player_->GetAttackCycle();
	worldTransform_.rotation_ = {};
}


void Hammer::AttackAnimationUpdate()
{
	if (AttackParameter_ < 1) {
		// １フレーム分
		float step = 1.0f / AttackCycle_;

		// フレームを加算
		AttackParameter_ += step;

		// ハンマーの始点と終点
		float weaponOriginRotation = std::numbers::pi_v<float> / 6; // ３０度
		float weaponEndRotation = abs( 2* std::numbers::pi_v<float> / 3 - weaponOriginRotation);

		worldTransform_.rotation_.x = weaponOriginRotation + EaseOutQuint(AttackParameter_) * weaponEndRotation;
	} else {
		player_->SetBehaviorRequest(Behavior::kRoot);
		worldTransform_.rotation_ = {};
	}

	//行列の更新と転送
	worldTransform_.UpdateMatrix();
}


Vector3 Hammer::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = {0.0f, 8.0f, 0.0f};
	// ワールド座標に変換
	Vector3 worldPos = Transform(offset, worldTransform_.matWorld_);
	return worldPos;
}

Vector3 Hammer::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

