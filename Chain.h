#pragma once
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
class Chain {
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
	void Initilaize(Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection"></param>
	void Draw(const ViewProjection& viewProjection);
	
private:
	//-------------メンバ変数-------------//
	
	//ワールド変換データ
	WorldTransform worldTransform_;

	//モデルデータ
	Model* model_ = nullptr;

};
