#pragma once
#include "Model.h"
#include "WorldTransform.h"

class Skydome
{
public:	//メンバ関数
	Skydome();
	~Skydome();
	void Initialize(Model* model);
	void Update();
	void Draw(ViewProjection& viewProjection);

private:
	//ワールド座標
	WorldTransform worldTransform_;

	//モデル
	Model* model_;

};
