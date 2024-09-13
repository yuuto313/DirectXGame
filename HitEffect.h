#pragma once
#include "Model.h"
#include "WorldTransform.h"

class HitEffect
{
public: //メンバ関数
	/// \brief 
	void Initialize(Model* model,Vector3 position, float lifeTime = defaultLfeTime_);
	/// \brief 
	void Update();
	/// \brief 
	void Draw(const ViewProjection& viewProjection);

public: //アクセッサ
	/// \brief ライフタイムの取得
	/// \return 
	float GetLifeTime() const { return lifeTime_; }

	

private: //メンバ変数
	//ワールド変換
	WorldTransform worldTransform_;

	//モデル
	Model* model_ = nullptr;

	//モデルの透明度
	float alpha_ = 0;

	//生存時間
	float lifeTime_ = 0.0f;

	//生存時間のデフォルト値
	static inline float defaultLfeTime_ = 30.0f;

	//周期
	float cycle_ = 0.0f;	

};
