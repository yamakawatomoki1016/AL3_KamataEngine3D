#include "Player.h"
#include "MyMath.h"
#include <numbers>
#include <algorithm>

using namespace KamataEngine::MathUtility;

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	//NULLポインタチェック
	assert(model);
	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;
	camera_ = camera;
	//ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	// アフィン変換行列を計算してメンバ変数に代入（前フレーム用）
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 定数バッファに転送
	worldTransform_.TransferMatrix();

	// 移動入力処理
	if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT) || KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

		// 加速処理（左右）
		KamataEngine::Vector3 acceleration = {};

		// 右キー入力
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {

			// 左向きから右向きに変えた場合
			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				// 急ブレーキ（逆方向入力時）
				velocity_.x *= kAttenuation;
			}

			acceleration.x += kAcceleration;
		}
		// 左キー入力
		else if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 右向きから左向きに変えた場合
			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				// 急ブレーキ（逆方向入力時）
				velocity_.x *= kAttenuation;
			}

			acceleration.x -= kAcceleration;
		}

		// 加速度分を速度に加算
		velocity_ += acceleration;

		// 最大速度制限
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	} else {
		// 非入力時は減衰
		constexpr float kDamping = 0.9f;
		velocity_ *= kDamping;

		// 微小速度はゼロに
		if (std::abs(velocity_.x) < 0.01f)
			velocity_.x = 0.0f;
		if (std::abs(velocity_.y) < 0.01f)
			velocity_.y = 0.0f;
		if (std::abs(velocity_.z) < 0.01f)
			velocity_.z = 0.0f;
	}

	// 位置更新
	worldTransform_.translation_ += velocity_;

	// 左右の自キャラ角度テーブル（右向きが π/2, 左向きが 3π/2）
	float destinationRotationYTable[] = {
	    std::numbers::pi_v<float> / 2.0f,       // 右向き（kRight）
	    std::numbers::pi_v<float> * 3.0f / 2.0f // 左向き（kLeft）
	};

	// Neutral状態での対処（例：旋回しない・現状維持）
	if (lrDirection_ != LRDirection::kNeutral) {
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_) - 1];

		// プレイヤーのY軸回転角度を設定
		worldTransform_.rotation_.y = destinationRotationY;
	}

	// 移動後の行列再計算＆転送
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}


void Player::Draw() { model_->Draw(worldTransform_, *camera_); }
