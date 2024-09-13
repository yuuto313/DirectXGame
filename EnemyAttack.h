#pragma once
#include "Collider.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"

class LockOn;
class EnemyAttack : public Collider{
public:
	// 攻撃パターン
	enum class Pattern {
		// 単体
		kSingle,
		// 全体
		kWhole,
	};

	//-------------基本処理-------------//

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model,const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Attack();

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="other"></param>
	void OnCollision([[maybe_unused]] Collider* other) override;

private:
	//-------------メンバ変数-------------//
	
	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// 攻撃パターン
	Pattern pattern_;

	// 存在しているか
	bool isActive_ = false;

};
