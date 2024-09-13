#include "CollisionManager.h"

#include "MyMath.h"

void CollisionManager::Reset()
{
	colliders_.clear();
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{
		//コライダーAの中心座標
	Vector3 positionA = colliderA->GetCenterPosition();
	//コライダーBの中心座標
	Vector3 positionB = colliderB->GetCenterPosition();

	//中心座標の差分
	Vector3 subtract = positionA - positionB;

	//中心座標の距離
	float distance = Length(subtract);

	//半径の合計
	float sumRadius = colliderA->GetCollisionRadius() + colliderB->GetCollisionRadius();

	//衝突判定
	if (distance < sumRadius)
	{
		//衝突時に呼ぶ関数を呼ぶ
		colliderA->OnCollision(colliderB);
		colliderB->OnCollision(colliderA);
	}

}

void CollisionManager::CheckAllCollision()
{
	//リスト内のペア総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (;itrA!= colliders_.end(); ++itrA)
	{
		Collider* colliderA = *itrA;

		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB)
		{
			Collider* colliderB = *itrB;

			//ペアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::AddCollider(Collider* collider)
{
	colliders_.push_back(collider);
}
