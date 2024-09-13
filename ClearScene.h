#pragma once
#include <Model.h>
#include "Sprite.h"
#include "Input.h"
#include "DirectXCommon.h"

#include "Fade.h"
//--------------------------------
// 仮実装
// ゲームシーンができてからシーンマネージャに組み込む
//--------------------------------

/// <summary>
/// クリアシーン
/// </summary>
class ClearScene {
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
	/// コンストラクタ
	/// </summary>
	ClearScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ClearScene();

	/// <summary>
	///	初期化
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

	/// <summary>
	/// UIの色を更新
	/// </summary>
	void UpdateUI();

	//-------------ゲッター・セッター-------------//

	/// <summary>
	/// 終了フラグのゲッター
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const { return finished_; }

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	//-------------ポインタ-------------//

	// フェード
	std::unique_ptr<Fade> fade_;

	// スプライト
	Sprite* clearSprite_ = nullptr;
	Sprite* uiSprite_ = nullptr;

	//-------------メンバ変数-------------//

	// 終了フラグ
	bool finished_ = false;

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	// クリアのテクスチャハンドル
	uint32_t clearTexID_ = 0;

	// UIのテクスチャハンドル
	uint32_t uiTexID_ = 0;

	// 存続時間(消滅までの時間)<秒>
	static inline const float kDuration = 3.0f;

	// 経過時間カウント
	float counter_ = 0.0f;

	// 増加フラグ
	bool isIncreasing_ = true;

	// UIの色
	Vector4 uiColor_;
};
