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
	skydomeModel_.reset(Model::CreateFromOBJ("Skydome", true));

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

	followCamera_->SetLockOn(lockOn_.get());

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

	//===================================================
	//プレイヤー
	//===================================================

	//モデル
	modelPlayerBody_.reset(Model::CreateFromOBJ("float_Body", true));
	modelPlayerHead_.reset(Model::CreateFromOBJ("float_Head", true));
	modelPlayerL_arm_.reset(Model::CreateFromOBJ("float_L_arm", true));
	modelPlayerR_arm_.reset(Model::CreateFromOBJ("float_R_arm", true));
	modelPlayerWeapon_.reset(Model::CreateFromOBJ("player_Weapon", true));
	modelHitEffect_.reset(Model::CreateFromOBJ("hitEffect", true));

	std::vector<Model*> playerModels = {
		modelPlayerBody_.get(),
		modelPlayerHead_.get(),
		modelPlayerL_arm_.get(),
		modelPlayerR_arm_.get(),
		modelPlayerWeapon_.get(),
		modelHitEffect_.get()
	};

	//生成
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels);

	//自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());
	//プレイヤーにs追従カメラのビュープロジェクションをセット
	player_->SetViewProjection(&followCamera_->GetViewProjection());
	//ロックオンカメラにプレイヤーをセット
	player_->SetLockOn(lockOn_.get());

	//===================================================
	//エネミー
	//===================================================

	modelEnemyBody_.reset(Model::CreateFromOBJ("enemy", true));
	modelEnemyWeapon_.reset(Model::CreateFromOBJ("enemy_Weapon", true));
	std::vector<Model*> enemyModels = {
		modelEnemyBody_.get(),
		modelEnemyWeapon_.get(),
		modelEnemyWeapon_.get()
	};

	const int numEnemies = 1;
	for (int i = 0; i < numEnemies; ++i) {
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
		enemy->Initialize(enemyModels);
		enemies_.push_back(std::move(enemy));
	}

	//===================================================
	//衝突判定マネージャー
	//===================================================

	collisionManager_ = std::make_unique<CollisionManager>();


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
	//プレイヤー
	//===================================================

	player_->Update();


	//===================================================
	//敵
	//===================================================

	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		enemy->Update();
	}

	//===================================================
	//衝突判定
	//===================================================

	//リストのクリア
	collisionManager_->Reset();
	//衝突判定を取りたいオブジェクトを登録
	collisionManager_->RegisterCollider(player_.get());
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		collisionManager_->RegisterCollider(enemy.get());
	}
	collisionManager_->RegisterCollider(player_->GetHammer());
	//衝突判定
	collisionManager_->CheckAllCollisions();



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
		// 追従カメラ
		lockOn_->Update(enemies_,viewProjection_);
		followCamera_->Update();
		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;

		// ビュープロジェクション行列の更新と転送
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

	//===================================================
	//プレイヤー
	//===================================================

	player_->Draw();

	//===================================================
	//エネミー
	//===================================================

	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		enemy->Draw(viewProjection_);
	}

	//===================================================
	//ロックオン
	//===================================================

	lockOn_->Draw();

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
