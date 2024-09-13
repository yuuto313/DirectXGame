#include "Chain.h"
#include <cassert>

Chain::Chain() {}

Chain::~Chain() {}

void Chain::Initilaize(Model* model) {
	//nullポインタチェック
	assert(model);

	//モデルデータに引数を代入
	model_ = model;

	//ワールド変換初期化
	worldTransform_.Initialize();

}

void Chain::Update()
{
	//ワールド変換更新
	worldTransform_.UpdateMatrix();

}

void Chain::Draw(const ViewProjection& viewProjection)
{ 
	model_->Draw(worldTransform_,viewProjection); 
}
