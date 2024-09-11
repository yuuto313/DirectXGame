#include "SceneManager.h"

void SceneManager::Initialize() {
	// ゲームシーンの初期化
	scene = Scene::kTitle;
	titleScene_ = std::make_unique<TitleScene>();
	titleScene_->Initialize();
}

void SceneManager::ChangeScene() {
	switch (scene) {
	case Scene::kUnknown:
		break;
	case Scene::kTitle:
		if (titleScene_->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			titleScene_.reset();
			// 新シーンの生成と初期化
			gameScene_ = std::make_unique<GameScene>();
			gameScene_->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene_->IsFinished()) {
			// シーン変更
			scene = Scene::kGameOver;
			// 旧シーンの解放
			gameScene_.reset();
			// 新シーンの生成と初期化
			gameoverScene_ = std::make_unique<GameOverScene>();
			gameoverScene_->Initialize();
		}
		break;
	case Scene::kGameOver:
		if (gameoverScene_->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			gameoverScene_.reset();
			// 新シーンの生成と初期化
			titleScene_ = std::make_unique<TitleScene>();
			titleScene_->Initialize();
		}
		break;
	default:
		break;
	}
}

void SceneManager::UpdateScene() {
	switch (scene) {
	case Scene::kUnknown:
		break;
	case Scene::kTitle:
		// タイトルシーンの更新
		titleScene_->Update();
		break;
	case Scene::kGame:
		// ゲームシーンの更新
		gameScene_->Update();
		break;
	case Scene::kGameOver:
		//ゲームオーバーシーンの更新
		gameoverScene_->Update();
		break;
	default:
		break;
	}
}

void SceneManager::DrawScene() {
	switch (scene) {
	case Scene::kUnknown:
		break;
	case Scene::kTitle:
		// タイトルシーンの描画
		titleScene_->Draw();
		break;
	case Scene::kGame:
		// ゲームシーンの描画
		gameScene_->Draw();
		break;
	case Scene::kGameOver:
		//ゲームオーバーシーンの描画
		gameoverScene_->Draw();
		break;
	default:
		break;
	}
}
