#include "TitleScene.h"
#include "WinApp.h"

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	//フェードの初期化
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	// フェードの持続時間
	float duration = 1.0f;
	//フェードの開始
	fade_->Start(Fade::Status::FadeIn, duration);
}

void TitleScene::Update() { 
	//--------------------------------
	// フェーズの切り替え
	//--------------------------------

	ChangePhase();

	//--------------------------------
	// フェードの更新
	//--------------------------------

	fade_->Update();
}

void TitleScene::Draw() {
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
	// フェードの描画
	//--------------------------------

	fade_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void TitleScene::ChangePhase() {
	switch (phase_) {
	case TitleScene::Phase::kFadeIn:
		// フェードの更新
		fade_->Update();

		// フェードインが終了したら、メインフェーズへ
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}

		break;
	case TitleScene::Phase::kMain:

		// メインフェーズ終了処理
		XINPUT_STATE joyState;
		Input::GetInstance()->GetJoystickState(0, joyState);
		// AボタンorSPACEキーでメインフェーズ終了
		if (input_->PushKey(DIK_SPACE) || joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			// フェードアウト開始
			float duration = 1.5f;
			fade_->Start(Fade::Status::FadeOut, duration);
			phase_ = Phase::kFadeOut;
		}

		break;
	case TitleScene::Phase::kFadeOut:
		// フェードの更新
		fade_->Update();

		// フェードアウトが終了したら、タイトルシーンを終了
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		break;
	default:
		break;
	}
}
