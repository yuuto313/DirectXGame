#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	/// <summary>
	/// モデル読み込みここから
	/// </summary>

	//天球
	skydomeModel_.reset(Model::CreateFromOBJ("Skydome",true));

	//地面
	groundModel_.reset(Model::CreateFromOBJ("ground", true));

	/// <summary>
	/// モデル読み込みここまで
	/// </summary>
	
	

	/// <summary>
	/// ゲームオブジェクトの初期化ここから
	/// </summary>


	//===================================================
	//ビュープロジェクション
	//===================================================

	viewProjection_.Initialize();
	viewProjection_.translation_ = Vector3(0.0f, 10.0f, -10.0f);

	//===================================================
	//デバッグカメラ
	//===================================================

	debugCamera_ = std::make_unique<DebugCamera>(WinApp::kWindowWidth, WinApp::kWindowHeight);

	//===================================================
	//追従カメラ
	//===================================================

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();


	//===================================================
	//ロックオンカメラ
	//===================================================

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	//===================================================
	//地面
	//===================================================

	ground_ = std::make_unique<Ground>();
	ground_->Initialize(groundModel_.get());

	//===================================================
	//天球
	//===================================================

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());

	/// <summary>
	/// ゲームオブジェクトの初期化ここまで
	/// </summary>

}

void GameScene::Update()
{
	/// <summary>
	/// ゲームオブジェクトの更新ここから
	/// </summary>

	//===================================================
	//地面
	//===================================================

	ground_->Update();

	//===================================================
	//天球
	//===================================================

	skydome_->Update();


	//===================================================
	//ビュープロジェクション
	//===================================================

	/*---------------------[カメラ]-----------------------*/

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_P)) {
		if (isDebugCameraActive_) {
			isDebugCameraActive_ = false;
		} else {
			isDebugCameraActive_ = true;
		}
	}
#endif
	
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
		viewProjection_.TransferMatrix();
	}

	/// <summary>
	/// ゲームオブジェクトの更新ここまで
	/// </summary>
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

	//===================================================
	//地面
	//===================================================

	ground_->Draw(viewProjection_);

	//===================================================
	//天球
	//===================================================

	skydome_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
