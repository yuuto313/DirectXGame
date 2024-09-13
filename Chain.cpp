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

void Chain::Initilaize(Model* model, Vector3 position) {
	//nullポインタチェック
	assert(model);

	//モデルデータに引数を代入
	model_ = model;

	//ワールド変換初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = { 2.0f,2.0f,2.0f };

	worldTransform_.UpdateMatrix();

	//種別IDの設定
	SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kChain));

	objColor_.Initialize();
	color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	preHp_ = GetHP();

}

void Chain::Update()
{
	//生存フラグがfalseの場合は何もしない
	if(!isAlive_)
	{
		return;
	}

	//ImGui::Begin("Chain");
	//ImGui::Text("hp %f", GetHP());
	//ImGui::Text("hit Count %d", hitCount_);
	//ImGui::End();

	//浮遊アニメーション
	floatAnimation();

	//色の更新
	ColorUpdate();

	//ワールド変換更新
	worldTransform_.UpdateMatrix();

}

void Chain::Draw(const ViewProjection& viewProjection)
{
	//生存フラグがfalseの場合は何もしない
	if(!isAlive_)
	{
		return;
	}

	model_->Draw(worldTransform_,viewProjection,&objColor_); 
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

void Chain::ColorUpdate()
{
	float alpha = 1.0f - (hitCount_ * 0.3f); // 1ヒットごとに30%ずつ薄くする
	if (alpha < 0.0f) alpha = 0.0f; // アルファ値が0未満にならないようにする

	color_.w = alpha;
	objColor_.SetColor(color_);
	objColor_.TransferMatrix();
}

void Chain::OnCollision([[maybe_unused]] Collider* other)
{
	if(GetHP() < preHp_)
	{
		hitCount_++;
		preHp_ = GetHP();
		if(hitCount_ >= 3)
		{
			isAlive_ = false;
		}
	}	
}

Vector3 Chain::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 8.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = Transform(offset, worldTransform_.matWorld_);
	return worldPos;
}

