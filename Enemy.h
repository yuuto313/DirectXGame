#pragma once
#include <vector>

#include "Collider.h"
#include "Model.h"
#include "WorldTransform.h"
#include "ShockWave.h"

class Player;
class LockOn;
class Enemy : public Collider
{
public:
	Enemy();
	void Initialize(const std::vector<Model*>& models,const Player* player);
	void Update();
	void Draw(const ViewProjection& viewProjection);
	/// \brief 移動
	void Move();

	/// \brief ターゲットに体を向ける
	void TurnToTarget();

	/*---------------------[衝撃波]-----------------------*/

	/// \brief 衝撃波の生成
	void GenerateShockWave();

	/// \brief 衝撃波の更新
	void UpdateShockWave();

	/// \brief 衝撃波の描画
	void DrawShockWave(const ViewProjection& viewProjection);


	/// \brief ワールド変換データの更新
	void UpdateMatrix();

	/// \brief ワールド座標を取得
	Vector3 GetWorldPosition();

	/// \brief モデルの中心座標を取得
	Vector3 GetCenterCoordinate() const;

	Vector3 GetCenterPosition() const override;

	/// \brief シリアルナンバーの取得
	/// \return 
	uint32_t GetSerialNumber() const { return serialNumber; }

	// 衝撃波のポインタ取得
	std::list<std::unique_ptr<ShockWave>>& GetShockWaves() { return shockWaves_; }

	/// \brief ワールド変換データの初期化
	void InitializeWorldTransform();

public:
	//// \brief 衝突時に呼び出される関数
	void OnCollision(Collider* other) override;

	/*---------------------[ステータス]-----------------------*/

	//ステータスの初期化
	void InitializeStatus();

	//生存フラグ
	bool IsAlive() { return isAlive_; }

	//速さ
	float GetSpeed() { return speed_; }
	void SetSpeed(float speed)
	{
		speed_ = speed;
		moveSpeed = { speed_,0.0f,0.0f };
	}

private:
	const int kModelIndexBody = 0;
	const int kModelIndexL_arm = 1;
	const int kModelIndexR_arm = 2;
	const int kModelIndexShockWave = 3;

	Vector3 moveSpeed = { speed_,0.0f,0.0f };

	const ViewProjection* viewProjection_ = nullptr;

	//パーツごとのワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	//モデル
	std::vector<Model*> models_;

	// ロックオン
	const Player* player_ = nullptr;

	// 衝撃波
	std::list<std::unique_ptr<ShockWave>> shockWaves_;

	/*---------------------[ステータス]-----------------------*/

	//生存フラグ
	bool isAlive_ = true;

	//速さ
	float speed_;

	uint32_t serialNumber = 0;

	//次のシリアルナンバー
	static uint32_t nextSerialNumber;

	//衝撃波発射フラグ
	bool isShot_ = false;

	//タイマー
	const float kCoolTimer_ = 30.0f;
	float timer_ = kCoolTimer_;



};
