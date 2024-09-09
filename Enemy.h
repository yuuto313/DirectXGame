#pragma once
#include "BaseCharacter.h"
class Enemy : public BaseCharacter
{
public:
	Enemy();
	void Initialize(const std::vector<Model*>& models) override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;

	/// \brief 移動
	void Move();

	/// \brief ワールド変換データの更新
	void UpdateMatrix();


	Vector3 GetCenterCoordinate()const;

	Vector3 GetCenterPosition() const override;

private:
	const int kModelIndexBody = 0;
	const int kModelIndexL_arm = 1;
	const int kModelIndexR_arm = 2;

	Vector3 kMoveSpeed = { 0.5f,0.0f,0.0f };

	const ViewProjection* viewProjection_ = nullptr;

	//パーツごとのワールド変換データ
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	//シリアルナンバー
	uint32_t serialNumber = 0;

	//次のシリアルナンバー
	static uint32_t nextSerialNumber;

};
