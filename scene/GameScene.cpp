#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	//解放処理
	delete uiSprite_;
	delete playerSpriteUI_;
	delete playerSpriteHP_;
	delete playerSpriteMP_;
	delete playerSkillSpPU_;
	delete playerSkillSpPD_;
	delete playerSkillSpSU_;
	delete playerSkillSpSD_;
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

	//===================================================
	// Player UI
	//===================================================

	playerTexUI_ = TextureManager::Load("playerUI/playerUI.png");
	playerSpriteUI_ = Sprite::Create(playerTexUI_, {});

	playerTexHP_ = TextureManager::Load("playerUI/playerHP.png");
	playerSpriteHP_ = Sprite::Create(playerTexHP_, {});

	playerTexMP_ = TextureManager::Load("playerUI/playerMP.png");
	playerSpriteMP_ = Sprite::Create(playerTexMP_, {});

	//===================================================
	// Player Skill UI 
	//===================================================

	playerSkillTexPU_ = TextureManager::Load("playerUI/sPowerUP.png");
	playerSkillSpPU_ = Sprite::Create(playerSkillTexPU_, {});

	playerSkillTexPD_ = TextureManager::Load("playerUI/sPowerDOWN.png");
	playerSkillSpPD_ = Sprite::Create(playerSkillTexPD_, {});

	playerSkillTexSU_ = TextureManager::Load("playerUI/sSpeedUP.png");
	playerSkillSpSU_ = Sprite::Create(playerSkillTexSU_, {});

	playerSkillTexSD_ = TextureManager::Load("playerUI/sSpeedDOWN.png");
	playerSkillSpSD_ = Sprite::Create(playerSkillTexSD_, {});


	/// <summary>
	/// モデル読み込みここから
	/// </summary>

	//===================================================
	//天球
	//===================================================
	
	skydomeModel_.reset(Model::CreateFromOBJ("Skydome", true));
	
	//===================================================
	//地面
	//===================================================

	groundModel_.reset(Model::CreateFromOBJ("ground", true));

	//===================================================
	//プレイヤー
	//===================================================

	modelPlayerBody_.reset(Model::CreateFromOBJ("float_Body", true));
	modelPlayerHead_.reset(Model::CreateFromOBJ("float_Head", true));
	modelPlayerL_arm_.reset(Model::CreateFromOBJ("float_L_arm", true));
	modelPlayerR_arm_.reset(Model::CreateFromOBJ("float_R_arm", true));
	modelPlayerWeapon_.reset(Model::CreateFromOBJ("player_Weapon", true));
	modelHitEffect_.reset(Model::CreateFromOBJ("hitEffect", true));
	modelShockWave_.reset(Model::CreateFromOBJ("wave", true));

	//===================================================
	//敵
	//===================================================

	modelEnemyBody_.reset(Model::CreateFromOBJ("enemy", true));
	modelEnemyWeapon_.reset(Model::CreateFromOBJ("enemy_Weapon", true));

	//===================================================
	//鎖
	//===================================================

	modelChain_.reset(Model::CreateFromOBJ("chain", true));

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
	std::vector<Model*> playerModels = {
		modelPlayerBody_.get(),
		modelPlayerHead_.get(),
		modelPlayerL_arm_.get(),
		modelPlayerR_arm_.get(),
		modelPlayerWeapon_.get(),
		modelHitEffect_.get(),
		modelShockWave_.get(),
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
	//鎖
	//===================================================

	chain_ = std::make_unique<Chain>();
	chain_->Initilaize(modelChain_.get());

	//===================================================
	//衝突判定マネージャー
	//===================================================

	collisionManager_ = std::make_unique<CollisionManager>();

	//===================================================
	//フェード
	//===================================================


	// フェードの初期化
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	// フェードの持続時間
	float duration = 2.0f;
	// フェードの開始
	fade_->Start(Fade::Status::FadeIn, duration);

	

	/// <summary>
	/// ゲームオブジェクトの初期化ここまで
	/// </summary>

}


void GameScene::Update()
{
	switch (phase_) {
	case GameScene::Phase::kFadeIn:
		// フェードの更新
		fade_->Update();

		if (fade_->IsFinished()) {
			// プレイフェーズへ
			phase_ = Phase::kPlay;
		}

		break;
	case GameScene::Phase::kPlay:

		/// <summary>
		/// ゲームオブジェクトの更新ここから
		/// </summary>

		//===================================================
		// 地面
		//===================================================

		ground_->Update();

		//===================================================
		// 天球
		//===================================================

		skydome_->Update();

		//===================================================
		// 鎖
		//===================================================

		chain_->Update();

		//===================================================
		// プレイヤー
		//===================================================

		player_->Update();

		//===================================================
		// 敵
		//===================================================

		for (const std::unique_ptr<Enemy>& enemy : enemies_) {
			enemy->Update();
		}

		//===================================================
		// 衝突判定
		//===================================================

		CheckAllCollision();

		//===================================================
		// ビュープロジェクション
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
			lockOn_->Update(enemies_, viewProjection_);
			followCamera_->Update();
			viewProjection_.matView = followCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の更新と転送
			viewProjection_.TransferMatrix();
		}

		//===================================================
		// ゲームオーバーシーンへの移行を確認するための仮実装
		//===================================================

		if (input_->PushKey(DIK_O) || !player_->IsAlive()) {
			// フェードアウト開始
			float duration = 1.5f;
			fade_->Start(Fade::Status::FadeOut, duration);
			phase_ = Phase::kFadeOutGameOver;
		}

		//===================================================
		// クリアシーンへの移行を確認するための仮実装
		//===================================================
		for (const std::unique_ptr<Enemy>& enemy : enemies_) {
			if (input_->PushKey(DIK_I) || !enemy->IsAlive()) {
				// フェードアウト開始
				float duration = 1.5f;
				fade_->Start(Fade::Status::FadeOut, duration);
				phase_ = Phase::kFadeOutClear;
			}
		}
		//===================================================
		// メニュー画面
		//===================================================

		XINPUT_STATE currentState;
		XINPUT_STATE previousState;

		// スタートボタンでメニュー画面を開く
		if (input_->GetJoystickState(0, currentState) && input_->GetJoystickStatePrevious(0, previousState)) {
			if (!(previousState.Gamepad.wButtons & XINPUT_GAMEPAD_START) && (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {
				if (pause_) {
					pause_ = false;
				} else {
					pause_ = true;
				}
			}
		}

		break;
	case GameScene::Phase::kFadeOutGameOver:
		//フェードの更新
		fade_->Update();

		//フェードアウトが終わったらゲームオーバーのフラグを立てる
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		break;
	case GameScene::Phase::kFadeOutClear:
		//フェードの更新
		fade_->Update();

		//フェードアウトが終わったらクリアのフラグを立てる
		if (fade_->IsFinished()) {
			isCleared_ = true;
		}

		break;
	default:
		break;
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
	//鎖
	//===================================================

	chain_->Draw(viewProjection_);

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

	


	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

	//===================================================
	//ロックオン
	//===================================================

	lockOn_->Draw();


#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// フェードの描画
	switch (phase_) {
	case GameScene::Phase::kFadeIn:
	case GameScene::Phase::kFadeOutGameOver:
	case GameScene::Phase::kFadeOutClear:
		//--------------------------------
		// フェードの描画
		//--------------------------------

		fade_->Draw();

		break;
	case GameScene::Phase::kPlay:
		//--------------------------------
		// UIの描画
		//--------------------------------
		
		uiSprite_->Draw();

		//===================================================
	    // Player UI
	    //===================================================

		playerSpriteUI_->Draw();
		playerSpriteHP_->Draw();
		playerSpriteMP_->Draw();


		break;
	default:
		break;
	}

	

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollision()
{
	//衝突マネージャーのリセット
	collisionManager_->Reset();

	//コライダーをリストに登録
	collisionManager_->AddCollider(player_.get());

	//コライダーリストに登録(ハンマー)
	collisionManager_->AddCollider(player_->GetHammer());

	//敵全てについて
	for (const std::unique_ptr<Enemy>& enemy : enemies_)
	{
		//コライダーをリストに登録
		collisionManager_->AddCollider(enemy.get());
	}

	//衝撃波の登録
	for (const std::unique_ptr<ShockWave>& shockWave : player_->GetShockWaves())
	{
		collisionManager_->AddCollider(shockWave.get());
	}

	//衝突判定と応答
	collisionManager_->CheckAllCollision();
}
