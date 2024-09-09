#include "Skydome.h"

#include <assert.h>

Skydome::Skydome()
{
}

Skydome::~Skydome()
{
}

void Skydome::Initialize(Model* model)
{
	//モデルの受け取ってメンバ変数に記録する
	assert(model);
	model_ = model;

	//ワールド座標の初期化
	worldTransform_.Initialize();

}

void Skydome::Update()
{
	//行列の更新
	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection)
{
	//モデルの描画
	model_->Draw(worldTransform_,viewProjection);
}
