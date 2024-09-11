#pragma once
#include "GameScene.h"
#include "TitleScene.h"
#include "GameOverScene.h"

//--------------------------------
//ゲームオーバーやクリアなど追加の画面はここへ
//--------------------------------

/// <summary>
/// シーンマネージャ
/// </summary>
class SceneManager {
public:
	// シーン
	enum class Scene {
		kUnknown = 0,
		// タイトル
		kTitle,
		// ゲーム
		kGame,
		// ゲームオーバー
		kGameOver,
		// クリア

	};

	//-------------基本処理-------------//

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// シーン切り替え処理
	/// </summary>
	void ChangeScene();

	/// <summary>
	/// シーンの更新
	/// </summary>
	void UpdateScene();

	/// <summary>
	/// シーンの描画
	/// </summary>
	void DrawScene();

private:
	//-------------ポインタ-------------//

	// タイトルシーン
	std::unique_ptr<TitleScene> titleScene_;

	// ゲームシーン
	std::unique_ptr<GameScene> gameScene_;

	//ゲームオーバーシーン
	std::unique_ptr<GameOverScene> gameoverScene_;
	
	//クリアシーン



	//-------------メンバ変数-------------//

	// 現在のシーン
	Scene scene = Scene::kUnknown;

};
