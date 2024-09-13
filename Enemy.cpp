#include "Enemy.h"

#include "CollisionConfig.h"
#include "CollisionTypeIdDef.h"
#include "EnemyStatusConfig.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include "ViewProjection.h"
#include "LockOn.h"
#include "Player.h"

uint32_t Enemy::nextSerialNumber = 0;


Enemy::Enemy()
{
	//シリアルナンバーを振る
	serialNumber = nextSerialNumber;
	//次の番号を加算
	nextSerialNumber++;
}

void Enemy::Initialize(const std::vector<Model*>& models,const Player* player)
{
	/*---------------------[ワールド変換データの初期化]-----------------------*/
	//Nullポインタチェック
	for (Model* model : models)
	{
		assert(model);
	}

	models_ = models;

	player_ = player;

	//ワールド変換初期化
	InitializeWorldTransform();

	//ステータスの初期化
	InitializeStatus();

	//ダメージを調整
	float damage = 5.0f;
	SetAttackPower(damage);

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
	
	if (!isShot_) {
		isShot_ = true;
		GenerateShockWave();
	} else {
		timer_--;
		if (timer_ <= 0) {
			isShot_ = false;
			timer_ = kCoolTimer_;
		}
	}

	
	UpdateShockWave();

	TurnToTarget();

	UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection)
{
	models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection);
	DrawShockWave(viewProjection);
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

void Enemy::TurnToTarget() {
	// ロックオン座標
	Vector3 lockOnPosition = player_->GetCenterCoordinate();
	// 追従対象からロックオン対象へのベクトル
	Vector3 sub = lockOnPosition - worldTransform_.translation_;
	// Y軸周り角度
	worldTransform_.rotation_.y = std::atan2(sub.x, sub.z);
}

void Enemy::GenerateShockWave() {
	// ロックオンされているならロックオン対象の方向に向かって衝撃波を生成

	// ターゲットの座標
	Vector3 targetPosition = player_->GetCenterCoordinate();
	// 高さの調整
	targetPosition.y -= 4.0f;
	// ロックオンの方向ベクトルを取得
	Vector3 velocity = targetPosition - worldTransform_.translation_;

	// 衝撃波を生成
	std::unique_ptr<ShockWave> shockWave = std::make_unique<ShockWave>();
	shockWave->Initialize(models_[kModelIndexShockWave], GetWorldPosition(), velocity, viewProjection_, GetAttackPower());
	shockWaves_.push_back(std::move(shockWave));
}

void Enemy::UpdateShockWave() {
	// 衝撃波を更新
	for (const std::unique_ptr<ShockWave>& shockwave : shockWaves_) {
		shockwave->Update();
	}

	// 無効な衝撃波を削除
	shockWaves_.remove_if([](const std::unique_ptr<ShockWave>& shockwave) { return !shockwave->IsActive(); });
}

void Enemy::DrawShockWave(const ViewProjection& viewProjection) {
	for (const std::unique_ptr<ShockWave>& shockwave : shockWaves_) {
		shockwave->Draw(viewProjection);
	}
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
	//衝突相手が敵ならダメージを受ける
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kHammer))
	{
		//ダメージを受ける
		TakeDamage(other->GetAttackPower());
	}

	//衝突相手の種別IDを取得
	typeID = other->GetTypeID();
	//衝突相手が敵ならダメージを受ける
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kShockWave))
	{
		//ダメージを受ける
		TakeDamage(other->GetAttackPower());
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
