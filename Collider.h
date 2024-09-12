#pragma once
#include "Model.h"
#include "Vector3.h"
#include "WorldTransform.h"

class Collider
{
public:
	virtual ~Collider() = default;

	//半径の取得
	float GetCollisionRadius() { return collisionRadius_; }
	//半径を設定
	void SetCollisionRadius(float radius) { collisionRadius_ = radius; }

	/// \brief 衝突時に呼ばれる関数
	virtual void OnCollision([[maybe_unused]]Collider* other) {}

	/// \brief 中心座標を取得
	/// \return 
	virtual Vector3 GetCenterPosition() const = 0;

	//種別IDの取得const
	uint32_t GetTypeID() const { return typeID_; }
	//種別IDの設定
	void SetTypeID(uint32_t typeID) { typeID_ = typeID; }

	float GetHP() { return hp_; }
	void SetHP(float hp) { hp_ = hp; }
	float GetAttackPower() { return attackPower_; }
	void SetAttackPower(float attackPower) { attackPower_ = attackPower; }

	void TakeDamage(float damage) { hp_ -= damage; }

private:
	//衝突半径
	float collisionRadius_ = 1.5f;

	WorldTransform worldTransform_;

	//種別ID
	uint32_t typeID_ = 0u;

	/*---------------------[ステータス]-----------------------*/

	//HP
	float hp_ = 100.0f;

	//攻撃力
	float attackPower_ = 10.0f;

};
