#pragma once
#include "Collider.h"
#include "Model.h"
#include "ShockWaveConfig.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class ShockWave : public Collider
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model,Vector3 position,Vector3 velocity,const ViewProjection* viewProjection);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

	void OnCollision() override;

public: // アクセッサ

	Vector3 GetWorldPosition() override;

	float GetDamage() { return damage_; };

	/// <summary>
	/// ショックウェーブの有効無効
	/// </summary>
	bool IsActive() const { return isActive_; }

private: // メンバ変数
	//ワールド変換データ
	WorldTransform worldTransform_;

	const ViewProjection* viewProjection_ = nullptr;

	Model* model_ = nullptr;

	//存在しているかどうか
	bool isActive_ = false;

	//移動方向
	Vector3 velocity_ = {};

	//移動速度
	float moveSpeed_ = kShockWaveMoveSpeed;

	//移動した距離
	float distanceTraveled_ = 0.0f;

	//ダメージ
	float damage_ = kShockWaveDamage;

private: // 静的メンバ変数
	//最大移動距離
	const float kMaxRange_ = kShockWaveRange;

};

