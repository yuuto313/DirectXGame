#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	//解放処理
	delete uiSprite_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//--------------------------------
	// テクスチャ読み込み
	//--------------------------------

	uiTexID_ = TextureManager::Load("operationR.png");

	//--------------------------------
	// 生成と初期化
	//--------------------------------

	uiSprite_ = Sprite::Create(uiTexID_, {});

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

		// ゲームオーバーシーンへの移行を確認するための仮実装
		// 誤作動を防ぐため同次押し込みで遷移

		if (input_->PushKey(DIK_SPACE) && input_->PushKey(DIK_RETURN)) {
			// フェードアウト開始
			float duration = 1.5f;
			fade_->Start(Fade::Status::FadeOut, duration);
			phase_ = Phase::kFadeOut;
		}

		//仮実装
		XINPUT_STATE joyState;
		Input::GetInstance()->GetJoystickState(0, joyState);
		// AボタンorSPACEキーでメインフェーズ終了
		if (input_->TriggerKey(DIK_TAB) || joyState.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
			if (pause_) {
				pause_ = false;
			} else {
				pause_ = true;
			}
		}

		break;
	case GameScene::Phase::kFadeOut:
		//フェードの更新
		fade_->Update();

		if (fade_->IsFinished()) {
			finished_ = true;
		}

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

		uiSprite_->Draw();

		break;
	default:
		break;
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
