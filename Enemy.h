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
	Vector3 GetWorldPosition() override;

	/// \brief モデルの中心座標を取得
	Vector3 GetCenterCoordinate() const;

	/// \brief ワールド変換データの初期化
	void InitializeWorldTransform();

public:
	//// \brief 衝突時に呼び出される関数
	void OnCollision() override;

	//ダメージを受ける
	void TakeDamage(float damage);

	/*---------------------[ステータス]-----------------------*/

	//ステータスの初期化
	void InitializeStatus();

	//生存フラグ
	bool IsAlive() { return isAlive_; }

	//HP
	float GetHp() { return hp_; }
	void SetHp(float hp) { hp_ = hp; }

	//速さ
	float GetSpeed() { return speed_; }
	void SetSpeed(float speed)
	{
		speed_ = speed;
		moveSpeed = { speed_,0.0f,0.0f };
	}

	//攻撃力
	float GetAttackPower() { return attackPower_; }
	void SetAttackPower(float attackPower) { attackPower_ = attackPower; }


private:
	const int kModelIndexBody = 0;
	const int kModelIndexL_arm = 1;
	const int kModelIndexR_arm = 2;

	Vector3 moveSpeed = { speed_,0.0f,0.0f };

	const ViewProjection* viewProjection_ = nullptr;

	//パーツごとのワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	//モデル
	std::vector<Model*> models_;

	/*---------------------[ステータス]-----------------------*/

	//生存フラグ
	bool isAlive_ = true;

	//HP
	float hp_;

	//速さ
	float speed_;

	//攻撃力
	float attackPower_;


};
