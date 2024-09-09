#include "CollisionManager.h"

void CollisionManager::Reset()
{
	colliders_.clear();
}

void CollisionManager::RegisterCollider(Collider* collider)
{
	colliders_.push_back(collider);
}

void CollisionManager::UnregisterCollider(Collider* collider)
{
	colliders_.remove(collider);
}

void CollisionManager::CheckAllCollisions()
{
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{
	// 衝突フィルタリング
	if ((colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) == 0 || (colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()) == 0) {
		return;
	}

	// コライダーAのワールド座標を取得
	Vector3 posA = colliderA->GetWorldPosition();

	// コライダーBのワールド座標を取得
	Vector3 posB = colliderB->GetWorldPosition();

	// 判定に必要な前計算
	float distance = Length(posA - posB);
	float combinedRadius = colliderA->GetRadius() + colliderB->GetRadius();

	// 球と球の衝突判定
	if (distance < combinedRadius) {
		// コライダーAの衝突時コールバックを呼び出す
		colliderA->OnCollision();
		// コライダーBの衝突時コールバックを呼び出す
		colliderB->OnCollision();
	}
}