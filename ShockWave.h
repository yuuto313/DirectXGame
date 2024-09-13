#pragma once
#include "Collider.h"
#include "HitEffect.h"
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
	void Initialize(std::vector<Model*> model,Vector3 position,Vector3 velocity,const ViewProjection* viewProjection, float damage);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

	void OnCollision([[maybe_unused]]Collider* other) override;

public: // アクセッサ

	Vector3 GetCenterPosition() const override;

	Vector3 GetWorldPosition();


	/// <summary>
	/// ショックウェーブの有効無効
	/// </summary>
	bool IsActive() const;

private: // メンバ変数
	//ワールド変換データ
	WorldTransform worldTransform_;

	const ViewProjection* viewProjection_ = nullptr;

	std::vector<Model*> models_;

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

	//ヒットエフェクトのリスト
	std::list<std::unique_ptr<HitEffect>> hitEffects_;

private: // 静的メンバ変数
	//最大移動距離
	const float kMaxRange_ = kShockWaveRange;

	const int kModelIndexWave = 0;
	const int kModelIndexEffect = 1;

};

