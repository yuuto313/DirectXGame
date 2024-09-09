#pragma once

//--------------------------------
// タイトルロゴやUIは後から追加
// 仮実装
//--------------------------------

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {
public:
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
	/// 終了フラグのゲッター
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const { return finished_; }

private:
	//終了フラグ
	bool finished_ = false;
};
