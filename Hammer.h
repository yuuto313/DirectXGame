#pragma once
#include "Collider.h"
#include "Model.h"
#include "HitEffect.h"

//プレイヤーのポインタを持つため前方宣言
class Player;
class Enemy;
class Hammer final : public Collider
{

public:	//メンバ関数
	void Initialize(std::vector<Model*> models,Player* player);

	/// \brief 更新
	void Update();

	void Draw(const ViewProjection& viewProjection);

	void DrawEffect(const ViewProjection& viewProjection);

	/*---------------------[衝突判定]-----------------------*/

	void OnCollision() override;

	void OnCollision(Collider* other);

	/*---------------------[攻撃]-----------------------*/

	//攻撃アニメーションの初期化
	void AttackAnimaitonInitialize();
	/// \brief 攻撃アニメーションの更新
	void AttackAnimationUpdate();

	/*---------------------[接触履歴]-----------------------*/


public: // アクセッサ
	const Vector3& GetRotaion() const { return worldTransform_.rotation_; }
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; }

	void SetParent(WorldTransform& worldTransform) { worldTransform_.parent_ = &worldTransform; }

	Vector3 GetCenterPosition() const;

	Vector3 GetWorldPosition() override;

private:
	//モデルデータ
	std::vector<Model*> models_;

	//モデル番号
	const int kIndexModelHammer = 0;
	const int kIndexModelEffect = 1;

	//ワールド変換データ
	WorldTransform worldTransform_;

	//プレイヤーのポインタ
	Player* player_ = nullptr;

	//エネミーのポインタ
	Enemy* enemy_ = nullptr;

	/*---------------------[ヒットエフェクト]-----------------------*/

	//ヒットエフェクトのリスト
	std::list<std::unique_ptr<HitEffect>> hitEffects_;

	/*---------------------[攻撃アニメーション]-----------------------*/

	// 攻撃ギミックの媒介変数
	float AttackParameter_;

	// 攻撃モーション周期
	uint16_t AttackCycle_;

};

