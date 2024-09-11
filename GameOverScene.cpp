#include "GameOverScene.h"
#include "TextureManager.h"
#include <algorithm>

GameOverScene::GameOverScene() {}

GameOverScene::~GameOverScene() {
	//解放処理
	delete gameoverSprite_;
	delete uiSprite_;
}

void GameOverScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	//--------------------------------
	// テクスチャ読み込み
	//--------------------------------
	// ゲームオーバー
	gameoverTexID_ = TextureManager::Load("gameover.png");

	// UI
	uiTexID_ = TextureManager::Load("titleA.png");
	uiColor_ = {1, 1, 1, 1};

	//--------------------------------
	// 生成と初期化
	//--------------------------------

	// フェードの生成と初期化
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();

	// スプライトの生成と初期化
	gameoverSprite_ = Sprite::Create(gameoverTexID_, {});
	uiSprite_ = Sprite::Create(uiTexID_, {});

	//--------------------------------
	// フェード関連
	//--------------------------------

	// フェードの持続時間
	float duration = 1.0f;
	// フェードの開始
	fade_->Start(Fade::Status::FadeIn, duration);

	// デバッグ時にゲームシーンからスタートさせるためtrueに
	// finished_ = true;
}

void GameOverScene::Update() {
	//--------------------------------
	// UIの更新
	//--------------------------------

	UpdateUI();

	//--------------------------------
	// フェーズの切り替え
	//--------------------------------

	ChangePhase();

	//--------------------------------
	// フェードの更新
	//--------------------------------

	fade_->Update();
}

void GameOverScene::Draw() {
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
	// タイトル描画
	//--------------------------------

	gameoverSprite_->Draw();

	//--------------------------------
	// UI描画
	//--------------------------------

	uiSprite_->Draw();

	//--------------------------------
	// フェードの描画
	//--------------------------------

	fade_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameOverScene::ChangePhase() {
	switch (phase_) {
	case GameOverScene::Phase::kFadeIn:
		// フェードの更新
		fade_->Update();

		// フェードインが終了したら、メインフェーズへ
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}

		break;
	case GameOverScene::Phase::kMain:

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
	case GameOverScene::Phase::kFadeOut:
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

void GameOverScene::UpdateUI() {
	if (isIncreasing_) {
		counter_ += 1.0f / 60.0f; // 1秒で60回呼び出されると仮定
		if (counter_ >= kDuration) {
			counter_ = kDuration;  // 上限に達したら固定
			isIncreasing_ = false; // 減少に転じる
		}
	} else {
		counter_ -= 1.0f / 60.0f; // 減少
		if (counter_ <= 0.0f) {
			counter_ = 0.0f;      // 下限に達したら固定
			isIncreasing_ = true; // 増加に転じる
		}
	}

	// ファードアウト
	uiColor_.w = std::clamp((kDuration - counter_) / kDuration, 0.0f, 1.0f);
	// 色変更オブジェクトに色の数値を設定する
	uiSprite_->SetColor(uiColor_);
}
