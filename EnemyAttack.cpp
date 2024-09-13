#include "EnemyAttack.h"

#include <cassert>

#include "MyMath.h"
#include "CollisionTypeIdDef.h"
#include "LockOn.h"

void EnemyAttack::Initialize(Model* model, const Vector3& position) {
	// NULLポインタチェック
	assert(model);

	//メンバ変数初期化
	//モデル
	model_ = model;

	//ワールド変換データ
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 識別IDを設定
	SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemyAttack));
}

void EnemyAttack::Update() {
	//--------------------------------
	// 攻撃処置
	//--------------------------------



	//--------------------------------
	// ワールド行列の更新と転送
	//--------------------------------

	worldTransform_.UpdateMatrix();


}

void EnemyAttack::Draw(const ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection); 
}

void EnemyAttack::Attack() {
	// 有効でない場合は何もしない
	if (!isActive_) {
		return;
	}

	switch (pattern_) {
	case EnemyAttack::Pattern::kSingle:
		// プレイヤーに向かって飛ばす



		break;
	case EnemyAttack::Pattern::kWhole:
		//自分の周りに飛ばす


		break;
	default:
		break;
	}

}

//void EnemyAttack::OnCollision(Collider* other) {
//	// 有効でない場合は何もしない
//	if (!isActive_) {
//		return;
//	}
//
//}
