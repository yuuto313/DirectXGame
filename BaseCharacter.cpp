#include "BaseCharacter.h"

void BaseCharacter::Initialize(const std::vector<Model*>& models)
{
	models_ = models;
	worldTransform_.Initialize();
	Collider::Initialize();
}

void BaseCharacter::Update()
{
	worldTransform_.UpdateMatrix();
}


void BaseCharacter::Draw(const ViewProjection& viewProjection)
{
	for (Model* model : models_)
	{
		model->Draw(worldTransform_, viewProjection);
	}
}

Vector3 BaseCharacter::GetCenterPosition() const
{
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
