#include "LockOn.h"

#include <cassert>

#include "ImGuiManager.h"
#include "Input.h"
#include "MyMath.h"
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WinApp.h"

void LockOn::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();

	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("./Resources/reticle.png");

	// スプライト生成
	lockOnMark_.reset(Sprite::Create(textureReticle, {640.0f, 360.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));
	assert(lockOnMark_);
}

void LockOn::Update(const std::list<std::unique_ptr<Enemy>>& enemies, const std::list<std::unique_ptr<Chain>>& chains, const ViewProjection& viewProjection)
{
	//ImGui::Begin("Lock On");
	//ImGui::DragFloat("Min Distance", &minDistance_);
	//ImGui::DragFloat("Maz Distance", &maxDistance_);
	//ImGui::End();

	XINPUT_STATE joyState;
	XINPUT_STATE preJoyState;
	Input::GetInstance()->GetJoystickState(0, joyState);
	if (Input::GetInstance()->GetJoystickStatePrevious(0, preJoyState))
	{
		printf("preJoyState is Active");
	}

	//ロックオン状態なら
	if (CanAttack(chains) && !chain_) {
		if (target_)
		{
			//エネミーが無効になった場合リセット
			if(!target_->IsAlive())
			{
				target_ = nullptr;
			}

			//ロックオン解除
			if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))
			{
				//ロックオンを外す
				target_ = nullptr;
			} else if (IsOutOfRange(viewProjection))
			{
				// ロックオンを外す
				target_ = nullptr;
			}

		} else {
			//ロックオンボタンをトリガーしたら
			if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))
			{
				//ロックオン対象の検索
				SearchLockOnTarget(enemies, chains, viewProjection);
			}

		}
		//ロックオン状態なら
		if (target_)
		{
			//ロックオンマークの座標計算
			Vector3 positionWorld = target_->GetCenterCoordinate();
			//ワールド座標からスクリーン座標に変換

			// ビューポート行列
			Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

			// ビュー行列とプロジェクション行列、ビューポート行列を合成する
			Matrix4x4 matViewProjectionViewport = (viewProjection.matView * viewProjection.matProjection) * matViewport;

			// ワールド→スクリーン座標変換
			Vector3 positionScreen = Transform(positionWorld, matViewProjectionViewport);

			Vector2 positionScreenV2(positionScreen.x, positionScreen.y);

			// スプライトのレティクルに座標設定
			lockOnMark_->SetPosition(positionScreenV2);

		}
	}else
	{
		if (chain_)
		{
			// チェーンが無効になった場合、次のチェーンを検索
			if (!chain_->IsAlive()) {
				chain_ = nullptr;
				SearchLockOnTarget(enemies, chains, viewProjection);
			}

			//ロックオン解除
			if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))
			{
				//ロックオンを外す
				chain_ = nullptr;
			} else if (IsOutOfRange(viewProjection))
			{
				// ロックオンを外す
				chain_ = nullptr;
			}

		} else {
			//ロックオンボタンをトリガーしたら
			if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && !(preJoyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))
			{
				//ロックオン対象の検索
				SearchLockOnTarget(enemies, chains, viewProjection);
			}

		}
		//ロックオン状態なら
		if (chain_)
		{
			//ロックオンマークの座標計算
			Vector3 positionWorld = chain_->GetCenterPosition();
			//ワールド座標からスクリーン座標に変換

			// ビューポート行列
			Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

			// ビュー行列とプロジェクション行列、ビューポート行列を合成する
			Matrix4x4 matViewProjectionViewport = (viewProjection.matView * viewProjection.matProjection) * matViewport;

			// ワールド→スクリーン座標変換
			Vector3 positionScreen = Transform(positionWorld, matViewProjectionViewport);

			Vector2 positionScreenV2(positionScreen.x, positionScreen.y);

			// スプライトのレティクルに座標設定
			lockOnMark_->SetPosition(positionScreenV2);

		}
	}
}

void LockOn::Draw()
{
	Sprite::PreDraw(dxCommon_->GetCommandList());

	if (target_ || chain_)
	{
		lockOnMark_->Draw();
	}

	Sprite::PostDraw();

}

void LockOn::SearchLockOnTarget(const std::list<std::unique_ptr<Enemy>>& enemies, const std::list<std::unique_ptr<Chain>>& chains ,const ViewProjection& viewProjection)
{
	

	/*-------------[ ロックオン対象の絞り込み(敵) ]--------------*/

	if (CanAttack(chains)) {
		//目標
		std::list<std::pair<float, const Enemy*>> targets;

		//すべての敵に対して順にロックオン判定
		for (const std::unique_ptr<Enemy>& enemy : enemies)
		{
			//敵のロックオン座標を取得
			Vector3 positionWorld = enemy->GetCenterCoordinate();

			//ワールド→ビュー座標変換
			Vector3 positionView = Transform(positionWorld, viewProjection.matView);

			//距離条件チェック
			if (minDistance_ <= positionView.z && positionView.z <= maxDistance_)
			{
				//カメラ前方との角度を計算
				float arcTangent = std::atan2(std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);

				//角度条件チェック(コーンに収まっているか)
				if (std::abs(arcTangent) <= angleRange_)
				{
					targets.emplace_back(std::make_pair(positionView.z, enemy.get()));
				}

			}
		}

		/*-------------[ 距離でソートしてロックオン ]--------------*/

		//ロックオン対象をリセット
		target_ = nullptr;
		if (!targets.empty())
		{
			//距離で昇順にソート
			targets.sort([](auto& pair1, auto& pair2) { return pair1.first < pair2.first; });
			target_ = targets.front().second;
		}

		return;
	}

	/*-------------[ ロックオン対象の絞り込み(鎖) ]--------------*/

	if (!chain_) {
		//目標
		std::list<std::pair<float, const Chain*>> targets;

		//すべての敵に対して順にロックオン判定
		for (const std::unique_ptr<Chain>& chain : chains)
		{
			if (!chain->IsAlive())
			{
				chain_ = nullptr;
				continue;
			}
			//敵のロックオン座標を取得
			Vector3 positionWorld = chain->GetCenterPosition();

			//ワールド→ビュー座標変換
			Vector3 positionView = Transform(positionWorld, viewProjection.matView);

			//距離条件チェック
			if (minDistance_ <= positionView.z && positionView.z <= maxDistance_)
			{
				//カメラ前方との角度を計算
				float arcTangent = std::atan2(std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);

				//角度条件チェック(コーンに収まっているか)
				if (std::abs(arcTangent) <= angleRange_)
				{
					targets.emplace_back(std::make_pair(positionView.z, chain.get()));
				}

			}

		}

		/*-------------[ 距離でソートしてロックオン ]--------------*/

		//ロックオン対象をリセット
		chain_ = nullptr;
		if (!targets.empty())
		{
			//距離で昇順にソート
			targets.sort([](auto& pair1, auto& pair2) { return pair1.first < pair2.first; });
			chain_ = targets.front().second;
		}
	}
}

bool LockOn::IsOutOfRange(const ViewProjection& viewProjection)
{
	/*-------------[ ロックオン座標取得 ]--------------*/
	if (target_)
	{
		// 目標
		std::list<std::pair<float, const Enemy*>> targets;

		// 敵のロックオン座標を取得
		Vector3 positionWorld = target_->GetCenterCoordinate();

		// ワールド→ビュー座標変換
		Vector3 positionView = Transform(positionWorld, viewProjection.matView);

		// 距離条件チェック
		if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
			// カメラ前方との角度を計算
			float arcTangent = std::atan2(std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);

			// 角度条件チェック(コーンに収まっているか)
			if (std::abs(arcTangent) <= angleRange_) {
				return false;
			}
		}
	}

	/*-------------[ ロックオン座標取得 ]--------------*/

	if (chain_)
	{
		// 目標
		std::list<std::pair<float, const Chain*>> chains;

		if (!chain_)
		{
			return true;
		}

		// 敵のロックオン座標を取得
		Vector3 positionWorld = chain_->GetCenterPosition();

		// ワールド→ビュー座標変換
		Vector3 positionView = Transform(positionWorld, viewProjection.matView);

		// 距離条件チェック
		if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
			// カメラ前方との角度を計算
			float arcTangent = std::atan2(std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);

			// 角度条件チェック(コーンに収まっているか)
			if (std::abs(arcTangent) <= angleRange_) {
				return false;
			}
		}
	}
	return true;
}

Vector3 LockOn::GetTargetPosition() const
{
	if (target_)
	{
		return target_->GetCenterCoordinate();
	}

	if(chain_)
	{
		return chain_->GetCenterPosition();
	}

	return Vector3{};
}

bool LockOn::CanAttack(const std::list<std::unique_ptr<Chain>>& chains)
{
	for (const std::unique_ptr<Chain>& chain : chains)
	{
		if (chain->IsAlive())
		{
			return false;
		}
	}

	return true;
}

