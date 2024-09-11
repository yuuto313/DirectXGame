#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "Fade.h"

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
		// フェードアウト
		kFadeOut,
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

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	
	//フェード
	std::unique_ptr<Fade> fade_;

	//現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	//終了フラグ
	bool finished_ = false;

	//ボーズフラグ
	bool pause_ = false;

};
