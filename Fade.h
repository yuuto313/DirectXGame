#pragma once
#include "DirectXCommon.h"
#include "Sprite.h"

/// <summary>
/// フェード
/// </summary>
class Fade {
public:
	// フェードの状態
	enum class Status {
		//フェードなし
		None,
		//フェードイン中
		FadeIn,
		//フェードアウト中
		FadeOut,
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
	/// フェード開始
	/// </summary>
	/// <param name="status"></param>
	/// <param name="duration"></param>
	void Start(Status status, float duration);

	/// <summary>
	/// フェード終了
	/// </summary>
	void Stop();

	/// <summary>
	/// フェード終了判定
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const;

private:
	//-------------ポインタ類-------------//
	// スプライト
	std::unique_ptr<Sprite> sprite_;

	//-------------変数-------------//
	// 現在のフェードの状態
	Status status_ = Status::None;

	// フェードの持続時間
	float duration_ = 0.0f;

	// 経過時間カウンター
	float counter_ = 0.0f;
};
