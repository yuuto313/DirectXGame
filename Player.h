#pragma once
#include <chrono>

#include "Model.h"
#include "WorldTransform.h"
#include <optional>

#include "Hammer.h"
#include "ShockWave.h"

enum class Behavior
{
	kRoot,		//通常状態
	kAttack,	//攻撃中
	kJump,
};

class ViewProjection;
class LockOn;
class Player : public Collider
{
public:

	void Initialize(const std::vector<Model*>& models);

	void Update();


	void Draw();

	/// \brief ワールド変換データの更新
	void UpdateMatrix();

	/*---------------------[初期化用関数]-----------------------*/

	/// \brief ワールド変換データの初期化
	void InitializeWorldTransform();

	/// \brief ハンマーの初期化
	void InitializeHammer();

	/// \brief ステータスの初期化
	void InitializeStatus();	
	
	/*---------------------[動作]-----------------------*/

	/// \brief 移動
	void Move();

	/// \brief 旋回
	void Turn();

	/// \brief 攻撃
	void Attack();

	/// \brief ターゲットに体を向ける
	void TurnToTarget();

	/// \brief スキルの初期化
	void SkillInitialize();

	/// \brief スキルの更新
	void SkillUpdate();

	/*---------------------[ImGui]-----------------------*/

	void UpdateImGui();

	/*---------------------[接触]-----------------------*/

	/// \brief 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* other) override;


	/*---------------------[振る舞い]-----------------------*/


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

	/// \brief ジャンプ行動初期化
	void BehaviorJumpInitialize();

	/// \brief ジャンプ行動更新
	void BehaviorJumpUpdate();

	/*---------------------[衝撃波]-----------------------*/

	/// \brief 衝撃波の生成
	void GenerateShockWave();

	/// \brief 衝撃波の更新
	void UpdateShockWave();

	/// \brief 衝撃波の描画
	void DrawShockWave();

	/*---------------------[アクセッサ]-----------------------*/

	/* ゲッター */
	/// \brief モデルの中心座標を取得
	Vector3 GetCenterCoordinate() const;

	//ワールド変換データの取得
	WorldTransform& GetWorldTransform() { return worldTransform_; }

	Vector3 GetWorldPosition();

	Vector3 GetCenterPosition() const override;

	//現在の行動状態の取得
	Behavior GetBehavior() { return behavior_; }

	/// \brief 攻撃アニメーションの周期を取得
	/// \return 
	uint16_t GetAttackCycle() { return AttackCycle_; }

	/// \brief ハンマーのポインタ取得
	/// \return 
	Hammer* GetHammer() { return hammer_.get(); }

	//衝撃波のポインタ取得
	std::list<std::unique_ptr<ShockWave>>& GetShockWaves() { return shockWaves_; }

	/// \brief 生存確認
	/// \return 
	bool IsAlive() { return isAlive_; }

	/* セッター */
	//カメラのセット
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	
	//ロックオンのセット
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	//次の振る舞いリクエスト
	void SetBehaviorRequest(Behavior behavior) { behaviorRequest_ = behavior; }

	/*---------------------[ステータス]-----------------------*/

	float GetSpeed() { return speed_; }
	void SetSpeed(float speed) { speed_ = speed; }

private:
	const int kModelIndexBody = 0;
	const int kModelIndexHead = 1;
	const int kModelIndexL_arm = 2;
	const int kModelIndexR_arm = 3;
	const int kModelindexWeapon = 4;
	const int kModelIndexEffect = 5;
	const int kModelIndexShockWave = 6;


	//ワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	//モデル
	std::vector<Model*> models_;

	//ハンマー
	std::unique_ptr<Hammer> hammer_;

	//衝撃波
	std::list<std::unique_ptr<ShockWave>> shockWaves_;

	//カメラ
	const ViewProjection* viewProjection_ = nullptr;

	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//振る舞い
	Behavior behavior_ = Behavior::kRoot;
	//次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	/*---------------------[ステータス]-----------------------*/

	//生存フラグ
	bool isAlive_ = true;

	//速さ
	float speed_;

	/*---------------------[移動]-----------------------*/

	//速度
	Vector3 velocity_ = {};

	/*---------------------[モーション]-----------------------*/

	/* 浮遊 */
	//浮遊ギミックの媒介変数
	float floatingParameter_;

	// 浮遊の振幅<m>
	float floatingAmplitude_;

	// 浮遊移動のサイクル
	uint16_t floatingCycle_;

	/* 攻撃 */
	//攻撃ギミックの媒介変数
	float AttackParameter_;

	//攻撃モーション
	uint16_t AttackCycle_;

	//スキルを発動した時間を記録
	std::chrono::steady_clock::time_point lastUpdateTime_;
	float skillDuration_; // 効果の持続時間（秒）
	float skillTimer_;
	bool skillActive_ = false;
	enum class EffectType { None, SpeedUp, AttackUp } currentEffect_ = EffectType::None;

	// 感情ゲージ
	float emotionGauge_;
	// 感情ゲージ消費量
	float emotionGaugeCost_; 

};
