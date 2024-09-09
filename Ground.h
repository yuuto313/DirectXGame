#pragma once
#include "Model.h"
#include "WorldTransform.h"

class ViewProjection;
class Ground
{
public:

	/// \brief 初期化
	void Initialize(Model* model);

	/// \brief 更新
	void Update();

	/// \brief 描画
	void Draw(ViewProjection& viewProjection);

private:
	//ワールド変換データ
	WorldTransform worldTransform_;

	//モデル
	Model* model_ = nullptr;
};

