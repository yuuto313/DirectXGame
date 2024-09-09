#pragma once
#include "WorldTransform.h"
#include  "ViewProjection.h"

class LockOn;
class FollowCamera
{
public:
	/// \brief 初期化
	void Initialize();

	/// \brief 更新
	void Update();

	void Follow();

	void Turn();

	/*---------------------[アクセッサ]-----------------------*/

	/* セッター */

	void SetTarget(const WorldTransform* target) { target_ = target; }

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	/* ゲッター */

	ViewProjection& GetViewProjection() { return viewProjection_; }


private:
	//ビュープロジェクション
	ViewProjection viewProjection_;

	//追従対象
	const WorldTransform* target_ = nullptr;

	//ロックオン
	const LockOn* lockOn_ = nullptr;

};
