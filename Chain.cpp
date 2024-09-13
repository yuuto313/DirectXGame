#include "Chain.h"
#include <cassert>
#include <numbers>

#include "CollisionTypeIdDef.h"
#include "ImGuiManager.h"
#include "MyMath.h"

uint32_t Chain::nextSerialNumber = 0;

Chain::Chain()
{
	//シリアルナンバーを振る
	serialNumber = nextSerialNumber;
	//次の番号を加算
	nextSerialNumber++;
}

Chain::~Chain() {}

void Chain::Initilaize(Model* model) {
	//nullポインタチェック
	assert(model);

	//モデルデータに引数を代入
	model_ = model;

	//ワールド変換初期化
	worldTransform_.Initialize();

	//種別IDの設定
	SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kChain));

}

void Chain::Update()
{
	ImGui::Begin("Chain");
	ImGui::Text("hp %f", GetHP());
	ImGui::End();

	//浮遊アニメーション
	floatAnimation();

	//ワールド変換更新
	worldTransform_.UpdateMatrix();

}

void Chain::Draw(const ViewProjection& viewProjection)
{ 
	model_->Draw(worldTransform_,viewProjection); 
}

void Chain::floatAnimation()
{
	const float step = 2.0f * std::numbers::pi_v<float> / floatingCycle_;

	// パラメータを1ステップ分加算
	floatingParameter_ += step;

	// 2πを超えたら0に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * std::numbers::pi_v<float>);

	worldTransform_.translation_.y = floatingAmplitude_ * std::sin(floatingParameter_);
}

void Chain::OnCollision(Collider* other)
{
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();

	// 
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy))
	{
		// ダメージを与える
		other->TakeDamage(GetAttackPower());
		// ダメージを受ける
	}
}

Vector3 Chain::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 1.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = Transform(offset, worldTransform_.matWorld_);
	return worldPos;
}

