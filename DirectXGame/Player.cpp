#define NOMINMAX
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

	if (onGround_) {
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
					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
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
					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
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

		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_UP)) {
			// ジャンプ初速
			velocity_ += KamataEngine::Vector3(0, kJumpAcceleration, 0);
		}
		// 着地フラグ
		bool landing = false;
		// 地面との当たり判定
		// 下降中？
		if (velocity_.y < 0) {
			// Y座標が地面以下になったら着地
			if (worldTransform_.translation_.y <= 1.0f) {
				landing = true;
			}
		}
		// 接地判定
		if (onGround_) {
			// ジャンプ開始
			if (velocity_.y > 0.0f) {
				// 空中状態に移行
				onGround_ = false;
			}
		} else {
			// 着地
			if (landing) {
				// めり込み排除
				worldTransform_.translation_.y = 1.0f;
				// 摩擦で横方向速度が減衰する
				velocity_.x *= (1.0f - kAttenuation);
				// 下方向速度をリセット
				velocity_.y = 0.0f;
				// 接地状態に移行
				onGround_ = true;
			}
		}
	} else {
		//落下速度
		velocity_ += KamataEngine::Vector3(0, -kGravityAcceleration, 0);
		//落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	//旋回制御
	//  旋回制御
	if (turnTimer_ > 0.0f) {
		// 1フレーム分時間を減らす
		turnTimer_ -= 1.0f / 60.0f;
		if (turnTimer_ < 0.0f)
			turnTimer_ = 0.0f;

		// 左右の自キャラ角度テーブル（右向きが π/2, 左向きが 3π/2）
		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,       // 右向き（kRight）
		    std::numbers::pi_v<float> * 3.0f / 2.0f // 左向き（kLeft）
		};

		if (lrDirection_ != LRDirection::kNeutral) {
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_) - 1];

			// t を 0.0 → 1.0 に進行させる
			float t = 1.0f - (turnTimer_ / kTimeTurn);
			t = std::clamp(t, 0.0f, 1.0f); // 安全対策

			worldTransform_.rotation_.y = EaseInOut(turnFirstRotationY_, destinationRotationY, t);
		}
	}

	// 地面の高さ（仮定値：Y=1.0f）
	constexpr float kGroundHeight = 1.0f;

	// 次フレームのY座標を先に計算して、地面との衝突をチェック
	float nextY = worldTransform_.translation_.y + velocity_.y;

	// 落下中かつ次フレームで地面を突き抜ける場合
	if (!onGround_ && nextY <= kGroundHeight) {
		// 着地処理
		worldTransform_.translation_.y = kGroundHeight;
		velocity_.y = 0.0f;
		velocity_.x *= (1.0f - kAttenuation); // 摩擦で横減衰
		onGround_ = true;
	} else {
		// 通常の位置更新
		worldTransform_.translation_ += velocity_;
	}

	// 移動後の行列再計算＆転送
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}


void Player::Draw() { model_->Draw(worldTransform_, *camera_); }
