#include "MenuScene.h"
#include "TextureManager.h"

MenuScene::MenuScene() {}

MenuScene::~MenuScene() {
	// 解放処理
	delete pauseMenuSprite_;
}

void MenuScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	//--------------------------------
	// テクスチャ読み込み
	//--------------------------------
	// メニュー(操作説明)
	pauseMenuTexID_ = TextureManager::Load("operation.png");
	texColor_ = {1, 1, 1, 1};

	//--------------------------------
	// 生成と初期化
	//--------------------------------

	// スプライトの生成と初期化
	pauseMenuSprite_ = Sprite::Create(pauseMenuTexID_, {});
}

void MenuScene::Update() {
	
}

void MenuScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	//--------------------------------
	// メニュー画面の描画
	//--------------------------------

	pauseMenuSprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
