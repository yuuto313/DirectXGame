#pragma once
#include "Sprite.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"

/// <summary>
/// 操作説明
/// </summary>
class MenuScene {
public:
	//-------------基本処理-------------//

	/// <summary>
	/// コンストラクタ
	/// </summary>
	MenuScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MenuScene();

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

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	//-------------ポインタ-------------//

	// スプライト
	Sprite* pauseMenuSprite_ = nullptr;

	//-------------メンバ変数-------------//

	// シーン終了フラグ
	bool finished_ = false;

	// 操作説明のテクスチャハンドル
	uint32_t pauseMenuTexID_ = 0;

	// テクスチャの色
	Vector4 texColor_;
};
