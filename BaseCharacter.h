#pragma once
#include <vector>

#include "Collider.h"
#include "Model.h"
#include "WorldTransform.h"

class BaseCharacter : public Collider
{
protected:	//メンバ変数
	//モデルデータ配列
	std::vector<Model*> models_;
	//ワールド変換データ
	WorldTransform worldTransform_;

public:
	/// \brief 初期化
	/// \param models モデルデータ配列
	virtual void Initialize(const std::vector<Model*>& models);

	/// \brief 更新
	virtual void Update();

	/// \brief 描画
	/// \param viewProjection ビュープロジェクション(参照渡し)
	virtual void Draw(const ViewProjection& viewProjection);

	/// \brief 中心座標を取得
	/// \return 
	virtual Vector3 GetCenterPosition() const override;

	/*---------------------[アクセッサ]-----------------------*/

	/// \brief ワールド変換データを取得
	/// \return ワールド変換データ
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

};
