#pragma once
#include <Model.h>
#include "Sprite.h"
#include "Input.h"
#include "DirectXCommon.h"

#include "Fade.h"
//--------------------------------
// タイトルロゴやUIは後から追加
// 仮実装
//--------------------------------

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {
public:
	// シーンのフェーズ
	enum class Phase {
		// フェードイン
		kFadeIn,
		// メイン部
		kMain,
		// フェードアウト
		kFadeOut,
	};

	//-------------基本処理-------------//

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	
	/// <summary>
	/// フェーズ切り替え処理
	/// </summary>
	void ChangePhase();

	//-------------ゲッター・セッター-------------//

	/// <summary>
	/// 終了フラグのゲッター
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const { return finished_; }

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	//-------------ポインタ類-------------//

	// フェード
	std::unique_ptr<Fade> fade_;

	//-------------変数-------------//

	// 終了フラグ
	bool finished_ = false;

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

};
