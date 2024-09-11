#include "Fade.h"
#include "WinApp.h"
#include <algorithm>

void Fade::Initialize() { 
	// スプライトの初期化
	sprite_ = std::make_unique<Sprite>(); 
	sprite_->Initialize();
	sprite_->SetSize(Vector2(WinApp::kWindowWidth, WinApp::kWindowHeight));
	// 黒を設定
	sprite_->SetColor(Vector4(0, 0, 0, 1));
}

void Fade::Update() {
	// フェード状態による分岐
	switch (status_) {
	case Fade::Status::None:
		// 何もしない
		break;
	case Fade::Status::FadeIn:
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.f;
		// フェード経過時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード経過時間に近づくほどα値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - (counter_ / duration_), 0.0f, 1.0f)));

		break;
	case Fade::Status::FadeOut:
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));

		break;
	default:
		break;
	}
}

void Fade::Draw() { 
	// フェード状態でなければ描画をスキップ
	if (status_ == Status::None) {
		return;
	}

	// スプライトの描画
	sprite_->Draw();
}

void Fade::Start(Status status, float duration) { 
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() { 
	status_ = Status::None; }

bool Fade::IsFinished() const { 
	// フェード状態による分岐
	switch (status_) {
	case Fade::Status::None:
		break;
	case Fade::Status::FadeIn:
	case Fade::Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
		break;
	default:
		break;
	}

	return true;
}
