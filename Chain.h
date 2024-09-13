#pragma once
#include "Collider.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"

//--------------------------------
// GameSceneからmodelを渡して描画させる
// 当たり判定は未実装
//--------------------------------

/// <summary>
/// 鎖
/// </summary>
class Chain : public Collider
{
public:
	//-------------基本処理-------------//
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Chain();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Chain();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initilaize(Model* model, Vector3 position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection"></param>
	void Draw(const ViewProjection& viewProjection);

	//浮遊アニメーション
	void floatAnimation();

	void ColorUpdate();

	void OnCollision(Collider* other) override;

	Vector3 GetCenterPosition() const override;

	uint32_t GetSerialNumber() const { return serialNumber; }

	bool IsAlive() { return isAlive_; }

private:
	//-------------メンバ変数-------------//

	bool isAlive_ = true;

	//ワールド変換データ
	WorldTransform worldTransform_;

	//モデルデータ
	Model* model_ = nullptr;

	//浮遊アニメーション用パラメータ
	float floatingParameter_ = 0.0f;
	float floatingCycle_ = 90.0f;
	float floatingAmplitude_ = 0.5f;

	uint32_t serialNumber = 1;

	//次のシリアルナンバー
	static uint32_t nextSerialNumber;

	//攻撃を受けた回数
	uint32_t hitCount_ = 0;

	//色変更オブジェクト
	ObjectColor objColor_;

	//色の数値
	Vector4 color_;

	float preHp_;

};
