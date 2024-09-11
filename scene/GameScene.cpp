#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>


GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// フェードの初期化
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	// フェードの持続時間
	float duration = 2.0f;
	// フェードの開始
	fade_->Start(Fade::Status::FadeIn,duration);

}

void GameScene::Update() {
	switch (phase_) {
	case GameScene::Phase::kFadeIn:
		//フェードの更新
		fade_->Update();

		if (fade_->IsFinished()){
			//プレイフェーズへ
			phase_ = Phase::kPlay;
		}

		break;
	case GameScene::Phase::kPlay:
		break;
	case GameScene::Phase::kFadeOut:
		//フェードの更新
		fade_->Update();

		break;
	default:
		break;
	}
}

void GameScene::Draw() {

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

	// フェードの描画
	switch (phase_) {
	case GameScene::Phase::kFadeIn:
	case GameScene::Phase::kFadeOut:

		fade_->Draw();

		break;
	case GameScene::Phase::kPlay:

		break;
	default:
		break;
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
