#include "Ground.h"

void Ground::Initialize(Model* model)
{
	//引数で受け取った物を取得
	model_ = std::move(model);
	
	worldTransform_.Initialize();
}

void Ground::Update()
{
	worldTransform_.UpdateMatrix();
}

void Ground::Draw(ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_,viewProjection);
}

