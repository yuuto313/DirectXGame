#include "HitEffect.h"

#include <cassert>

#include "MyMath.h"


void HitEffect::Initialize(Model* model,Vector3 position, float lifeTime)
{
	//Nullポインタチェック
	assert(model);

	//引数で受け取ったものを取得
	model_ = model;

	//初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = { 2.0f,2.0f,2.0f };
	worldTransform_.UpdateMatrix();

	//生存時間の設定
	lifeTime_ = lifeTime;

	//生存時間を周期とする
	cycle_ = lifeTime_;

	//透明度の初期化
	alpha_ = 255.0f;

	//モデルの透明度を設定
	model_->SetAlpha(alpha_);
	
}

void HitEffect::Update()
{
	//生存時間を減らす
	lifeTime_--;

	//１フレームでどのくらい拡大していくか
	float scaleRate = 0.1f;

	//スケールを拡大していく
	worldTransform_.scale_ += Vector3(scaleRate,scaleRate,scaleRate);

	//１フレームあたりの減算率
	float alphaRate = 255 / cycle_;

	//透明度を減らしていく
	alpha_ -= alphaRate;

	//透明度セット
	model_->SetAlpha(alpha_);

	worldTransform_.UpdateMatrix();

}

void HitEffect::Draw(const ViewProjection& viewProjection)
{
	//モデルの描画
	model_->Draw(worldTransform_, viewProjection);
}

