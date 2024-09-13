#pragma once

#include "Audio.h"
#include "CollisionManager.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "FollowCamera.h"
#include "Ground.h"
#include "Input.h"
#include "Model.h"
#include "Skydome.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "LockOn.h"
#include "Player.h"

#include "Fade.h"
#include "Chain.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数

	//シーンのフェーズ
	enum class Phase {
		// フェードイン
		kFadeIn,
		// ゲームプレイ
		kPlay,
		// ゲームオーバー時のフェードアウト
		kFadeOutGameOver,
		//クリア時のフェードアウト
		kFadeOutClear,
	};

	//-------------基本処理-------------//

	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void CheckAllCollision();

	//敵に攻撃可能か判定するフラグ
	void CheckCanAttackEnemy();

	//終了条件をチェック
	void CheckEndCondition();

	//-------------ゲッター・セッター-------------//

	/// <summary>
	/// 終了フラグのゲッター
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const { return finished_; }

	/// <summary>
	/// ポーズのフラグゲッター
	/// </summary>
	/// <returns></returns>
	bool IsPaused() const { return pause_; }

	/// <summary>
	/// クリアフラグのゲッター
	/// </summary>
	/// <returns></returns>
	bool IsCleared() const { return isCleared_; }

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;


	/// <summary>
	/// ゲームシーン用
	/// </summary>

	//===================================================
	//ビュープロジェクション
	//===================================================

	ViewProjection viewProjection_;

	//===================================================
	//デバッグカメラ
	//===================================================

	std::unique_ptr<DebugCamera> debugCamera_;
	//デバッグカメラ切り替え
	bool isDebugCameraActive_ = false;

	//===================================================
	//追従カメラ
	//===================================================

	std::unique_ptr<FollowCamera> followCamera_;

	//===================================================
	//ロックオンカメラ
	//===================================================

	std::unique_ptr<LockOn> lockOn_;

	//===================================================
	//天球
	//===================================================

	std::unique_ptr<Skydome> skydome_;
	//モデル
	std::unique_ptr<Model> skydomeModel_;

	//===================================================
	//地面
	//===================================================

	std::unique_ptr<Ground> ground_;
	//モデル
	std::unique_ptr<Model> groundModel_;

	//===================================================
	//プレイヤー
	//===================================================

	std::unique_ptr<Player> player_;
	//モデル
	std::unique_ptr<Model>modelPlayerBody_;
	std::unique_ptr<Model>modelPlayerHead_;
	std::unique_ptr<Model>modelPlayerL_arm_;
	std::unique_ptr<Model>modelPlayerR_arm_;
	std::unique_ptr<Model>modelPlayerWeapon_;
	std::unique_ptr<Model>modelHitEffect_;
	std::unique_ptr<Model>modelShockWave_;


	//===================================================
	//エネミー
	//===================================================

	std::list<std::unique_ptr<Enemy>> enemies_;
	//モデル
	std::unique_ptr<Model>modelEnemyBody_;
	std::unique_ptr<Model>modelEnemyWeapon_;

	//===================================================
	//鎖
	//===================================================

	std::list<std::unique_ptr<Chain>> chain_;
	//モデル
	std::unique_ptr<Model>modelChain_;

	//===================================================
	//衝突マネージャー
	//===================================================

	std::unique_ptr<CollisionManager> collisionManager_;

	//===================================================
	//フェード
	//===================================================

	std::unique_ptr<Fade> fade_;
  
	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	// 終了フラグ
	bool finished_ = false;

	// クリアフラグ
	bool isCleared_ = false;

	// ボーズフラグ
	bool pause_ = false;
  
	//===================================================
	// UI
	//===================================================

	 // 操作説明UI
	Sprite* uiSprite_ = nullptr;

	// UIテクスチャ
	uint32_t uiTexID_ = 0;

};
