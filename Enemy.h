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

	/// \brief ワールド座標を取得
	Vector3 GetWorldPosition();

	/// \brief モデルの中心座標を取得
	Vector3 GetCenterCoordinate() const;

	Vector3 GetCenterPosition() const override;

	/// \brief シリアルナンバーの取得
	/// \return 
	uint32_t GetSerialNumber() const { return serialNumber; }

	/// \brief ワールド変換データの初期化
	void InitializeWorldTransform();

public:
	//// \brief 衝突時に呼び出される関数
	void OnCollision(Collider* other) override;

	/*---------------------[ステータス]-----------------------*/

	//ステータスの初期化
	void InitializeStatus();

	//生存フラグ
	bool IsAlive() const{ return isAlive_; }

	//攻撃可能のフラグ
	bool GetCanAttack() { return canAttack_; }
	void SetCanAttack(bool canAttack) { canAttack_ = canAttack; }

	//速さ
	float GetSpeed() { return speed_; }
	void SetSpeed(float speed)
	{
		speed_ = speed;
		moveSpeed = { speed_,0.0f,0.0f };
	}

private:
	const int kModelIndexBody = 0;
	const int kModelIndexL_arm = 1;
	const int kModelIndexR_arm = 2;
	const int kModelIndexBarrier = 3;

	Vector3 moveSpeed = { speed_,0.0f,0.0f };

	const ViewProjection* viewProjection_ = nullptr;

	//パーツごとのワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	WorldTransform worldTransformBarrier_;

	//モデル
	std::vector<Model*> models_;

	//バリアの色
	ObjectColor barrierColor_;
	Vector4 barrierColorValue_ = { 1.0f,1.0f,1.0f,1.0f };

	/*---------------------[ステータス]-----------------------*/

	//生存フラグ
	bool isAlive_ = true;

	//速さ
	float speed_;

	uint32_t serialNumber = 0;

	//次のシリアルナンバー
	static uint32_t nextSerialNumber;

	//攻撃可能のフラグ
	bool canAttack_ = false;

};
