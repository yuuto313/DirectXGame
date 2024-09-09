#pragma once
#include "Collider.h"
#include <list>
#include "MyMath.h"

class CollisionManager {
public:
	//リセット
	void Reset();
	// コライダーの登録
	void RegisterCollider(Collider* collider);
	// コライダーの解除
	void UnregisterCollider(Collider* collider);
	// 全てのコライダー間の衝突判定を行う
	void CheckAllCollisions();

private:
	// コライダー同士の衝突判定
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);
	// コライダーリスト
	std::list<Collider*> colliders_;
};