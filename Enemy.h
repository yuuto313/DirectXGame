#pragma once
#include <vector>

#include "Collider.h"
#include "Model.h"
#include "WorldTransform.h"

class Enemy : public Collider
{
public:
	Enemy();
	void Initialize(const std::vector<Model*>& models) ;
	void Update();
	void Draw(const ViewProjection& viewProjection);
	/// \brief 移動
	void Move();

	/// \brief ワールド変換データの更新
	void UpdateMatrix();

	Vector3 GetWorldPosition() override;

	Vector3 GetCenterCoordinate() const;

private:
	const int kModelIndexBody = 0;
	const int kModelIndexL_arm = 1;
	const int kModelIndexR_arm = 2;

	Vector3 kMoveSpeed = { 0.5f,0.0f,0.0f };

	const ViewProjection* viewProjection_ = nullptr;

	//パーツごとのワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	//モデル
	std::vector<Model*> models_;

};
