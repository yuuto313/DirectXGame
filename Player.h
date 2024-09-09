#pragma once
#include "BaseCharacter.h"
#include "Model.h"
#include "WorldTransform.h"
#include <optional>

#include "Hammer.h"

enum class Behavior
{
	kRoot,		//通常状態
	kAttack,	//攻撃中
	kJump,
};

class ViewProjection;
class LockOn;
class Player : public BaseCharacter
{
public:

	void Initialize(const std::vector<Model*>& models,ViewProjection* viewProjection);

	void Upadate();

	void Draw();

	/// \brief ワールド変換データの更新
	void UpdateMatrix();

	/// \brief 移動
	void Move();

	/// \brief 旋回
	void Turn();

	/// \brief 攻撃
	void Attack();


	/// \brief ターゲットに体を向ける
	void TurnToTarget();

	void UpdateImGui();

	void InitializeFloatingGimick();

	/// \brief 浮遊ギミック更新
	void UpdateFloatingGimick();

	/// \brief 通常行動状態の初期化
	void BehaviorRootInitialize();

	/// \brief 通常行動更新
	void BehaviorRootUpdate();

	/// \brief 攻撃行動状態の初期化
	void BehaviorAttackInitialize();

	/// \brief 攻撃行動更新
	void BehaviorAttackUpdate();

	/// \brief 調整項目の適用
	void ApplyParameters();

	/// \brief ジャンプ行動初期化
	void BehaviorJumpInitialize();

	/// \brief ジャンプ行動更新
	void BehaviorJumpUpdate();

	/// \brief 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	/*---------------------[アクセッサ]-----------------------*/

	/* ゲッター */

	//ワールド変換データの取得
	WorldTransform& GetWorldTransform() { return worldTransform_; }

	//中心座標を取得
	Vector3 GetCenterPosition() const override;

	//現在の行動状態の取得
	Behavior GetBehavior() { return behavior_; }

	/// \brief 攻撃アニメーションの周期を取得
	/// \return 
	uint16_t GetAttackCycle() { return AttackCycle_; }

	/// \brief ハンマーのポインタ取得
	/// \return 
	Hammer* GetHammer() { return hammer_.get(); }

	/* セッター */

	
	//ロックオンのセット
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	//次の振る舞いリクエスト
	void SetBehaviorRequest(Behavior behavior) { behaviorRequest_ = behavior; }

private:
	const int kModelIndexBody = 0;
	const int kModelIndexHead = 1;
	const int kModelIndexL_arm = 2;
	const int kModelIndexR_arm = 3;
	const int kModelindexWeapon = 4;
	const int kModelIndexEffect = 5;

	//ワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	//モデル
	Model* modelHead_ = nullptr;
	Model* modelBody_ = nullptr;
	Model* modelL_arm_ = nullptr;
	Model* modelR_arm_ = nullptr;
	Model* modelWeapon = nullptr;

	//ハンマー
	std::unique_ptr<Hammer> hammer_;

	//カメラ
	ViewProjection* viewProjection_ = nullptr;

	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//振る舞い
	Behavior behavior_ = Behavior::kRoot;
	//次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	/*---------------------[移動]-----------------------*/

	//速度
	Vector3 velocity_ = {};

	//速さ
	float speed = 0.3f;

	/*---------------------[浮遊]-----------------------*/

	//浮遊ギミックの媒介変数
	float floatingParameter_;

	// 浮遊の振幅<m>
	float floatingAmplitude_;

	// 浮遊移動のサイクル
	uint16_t floatingCycle_;

	/*---------------------[攻撃]-----------------------*/

	//攻撃ギミックの媒介変数
	float AttackParameter_;

	//攻撃モーション
	uint16_t AttackCycle_;

};
