#include "Enemy.h"

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
	for (Model* model : models)
	{
		assert(model);
	}

	models_ = models;

	//ワールド変換初期化
	InitializeWorldTransform();

	//ステータスの初期化
	InitializeStatus();

	/*---------------------[種別IDの設定]-----------------------*/

	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));

}

void Enemy::Update()
{
	ImGui::Begin("Enemy");
	float hp = GetHP();
	ImGui::Text("HP %f",hp, 0.0f, 100.0f);
	ImGui::SliderFloat3("Base Translation", &worldTransform_.translation_.x, -20.0f, 20.0f);
	ImGui::SliderFloat3("ArmL Translation",&worldTransformL_arm_.translation_.x,-20.0f,20.0f);
	ImGui::SliderFloat3("ArmR Translation",&worldTransformR_arm_.translation_.x,-20.0f,20.0f);
	ImGui::NewLine();
	ImGui::SliderFloat("Move Speed", &moveSpeed.x, 0.0f, 3.0f);
	ImGui::End();

	Move();
	UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection)
{
	models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection);
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
	worldTransform_.translation_ = { 0.0f, 4.0f, 100.0f };


	//パーツも
	worldTransformBody_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.translation_ = { 2.6f, 0.0f, 0.0f };
	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.translation_ = { -2.6f, 0.0f, 0.0f };

	//親子関係を結ぶ
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
}

void Enemy::OnCollision(Collider* other)
{
	//衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手がプレイヤー以外なら消す
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayer))
	{
		//ダメージを与える
		other->TakeDamage(GetAttackPower());
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
