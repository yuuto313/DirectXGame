#pragma once
#include <list>
#include <memory>

#include "Collider.h"
#include "Model.h"

class CollisionManager
{
public:	
	/// \brief リストのリセット
	void Reset();

	/// \brief コライダー２つの衝突判定と応答
	/// \param colliderA 
	/// \param colliderB 
	void CheckCollisionPair(Collider* colliderA,Collider* colliderB);

	/// \brief 全ての当たり判定チェック
	void CheckAllCollision();

	void AddCollider(Collider* collider);

private:
	//コライダー
	std::list<Collider*> colliders_;
};
