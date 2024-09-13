#pragma once
#include <memory>
#include <numbers>

#include "Chain.h"
#include "DirectXCommon.h"

#include "Enemy.h"
#include "Sprite.h"

class LockOn
{
public:
	/// \brief 初期化
	void Initialize();

	/// \brief 更新
	void Update(const std::list<std::unique_ptr<Enemy>>& enemies,const ViewProjection& viewProjection);

	/// \brief 描画
	void Draw();

	/// \brief ロックオン対象を検索
	void SearchLockOnTarget(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);

	/// \brief 範囲外判定
	/// \return 
	bool IsOutOfRange(const ViewProjection& viewProjection);

	/// \brief ロックオン対象の座標取得
	/// \return 
	Vector3 GetTargetPosition() const;

	/// \brief ロックオン中かどうか
	/// \return 
	bool ExistTarget() const { return target_ ? true : false; }

	const Enemy* GetTarget() const { return target_; }

private:
	DirectXCommon* dxCommon_ = nullptr;

	std::unique_ptr<Sprite> lockOnMark_;

	//ロックオン対象
	const Enemy* target_ = nullptr;
	const Chain* chain_ = nullptr;

	const float kDegreeToRadian_ = std::numbers::pi_v<float> / 180.0f;
	// 最小距離
	float minDistance_ = 10.0f;
	// 最大距離
	float maxDistance_ = 500.0f;

	//角度範囲
	float angleRange_ = 20.0f * kDegreeToRadian_;

};
