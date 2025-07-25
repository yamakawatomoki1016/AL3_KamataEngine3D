#define NOMINMAX
#include "Player.h"
#include "MyMath.h"
#include <numbers>
#include <algorithm>
#include "MapChipField.h"

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

void Player::InputMove() {
	if (onGround_) {
		// 移動入力処理
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT) || KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {
			KamataEngine::Vector3 acceleration = {};

			if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					velocity_.x *= kAttenuation;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x += kAcceleration;
			} else if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					velocity_.x *= kAttenuation;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x -= kAcceleration;
			}

			velocity_ += acceleration;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			constexpr float kDamping = 0.9f;
			velocity_ *= kDamping;

			if (std::abs(velocity_.x) < 0.01f)
				velocity_.x = 0.0f;
			if (std::abs(velocity_.y) < 0.01f)
				velocity_.y = 0.0f;
			if (std::abs(velocity_.z) < 0.01f)
				velocity_.z = 0.0f;
		}

		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_UP)) {
			velocity_ += KamataEngine::Vector3(0, kJumpAcceleration, 0);
		}

		bool landing = false;
		if (velocity_.y < 0 && worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
		if (onGround_ && velocity_.y > 0.0f) {
			onGround_ = false;
		} else if (!onGround_ && landing) {
			worldTransform_.translation_.y = 1.0f;
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	} else {
		velocity_ += KamataEngine::Vector3(0, -kGravityAcceleration, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

}

bool Player::CheckCollisionUp(CollisionMapInfo& info) {
	// 移動後の４つの角の座標
	std::array<KamataEngine::Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount_, static_cast<Corner>(i));
	}
	// 上昇あり？
	if (info.moveAmount_.y <= 0.0f) {
		return false;
	}
	// ヒット判定フラグ
	bool hit = false;

	// 左上チェック
	{
		IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
		MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			hit = true;
		}
	}

	// 右上チェック
	{
		IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
		MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			hit = true;
		}
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.moveAmount_ + KamataEngine::Vector3(0, +kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount_.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		info.isHitCeiling_ = true;
	}
	return hit;
}

// bool Player::CheckCollisionDown(const CollisionMapInfo& info) { return false; }
//
// bool Player::CheckCollisionLeft(const CollisionMapInfo& info) { return false; }
//
// bool Player::CheckCollisionRight(const CollisionMapInfo& info) { return false; }

void Player::CheckMapCollision(CollisionMapInfo& info) {
	// 上方向の当たり判定
	info.isHitCeiling_ = CheckCollisionUp(info);

	//// 下方向の当たり判定
	//info.isOnGround_ = CheckCollisionDown(info);

	//// 左方向の当たり判定
	//bool hitLeft = CheckCollisionLeft(info);

	//// 右方向の当たり判定
	//bool hitRight = CheckCollisionRight(info);

	//// 左右どちらかでも当たってたら壁接触フラグON
	//info.isHitWall_ = hitLeft || hitRight;
}

KamataEngine::Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) { 
	KamataEngine::Vector3 offsetTable[kNumCorner] = {
		{+kWidth / 2.0f, -kHeight / 2.0f, 0}, //kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, //kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}, //LeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::ApplyCollisionResult(const CollisionMapInfo& info) {
	//移動
	worldTransform_.translation_ += info.moveAmount_;
}

void Player::ResolveCeilingCollision(const CollisionMapInfo& info) {
	//天井に当たった？
	if (info.isHitCeiling_) {
		velocity_.y = 0.0f;
	}
}

void Player::Update() {
	// 移動処理を分離
	InputMove();

	//衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	//移動量に速度の値をコピー
	collisionMapInfo.moveAmount_ = velocity_;
	//マップ衝突チェック
	CheckMapCollision(collisionMapInfo);
	ResolveCeilingCollision(collisionMapInfo);
	ApplyCollisionResult(collisionMapInfo);

	// 旋回制御などはUpdateに残す
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		if (turnTimer_ < 0.0f)
			turnTimer_ = 0.0f;

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		if (lrDirection_ != LRDirection::kNeutral) {
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_) - 1];
			float t = 1.0f - (turnTimer_ / kTimeTurn);
			t = std::clamp(t, 0.0f, 1.0f);
			worldTransform_.rotation_.y = EaseInOut(turnFirstRotationY_, destinationRotationY, t);
		}
	}

	// 座標修正（地面と接触したときの補正）
	constexpr float kGroundHeight = 1.0f;
	float nextY = worldTransform_.translation_.y + velocity_.y;
	if (!onGround_ && nextY <= kGroundHeight) {
		worldTransform_.translation_.y = kGroundHeight;
		velocity_.y = 0.0f;
		velocity_.x *= (1.0f - kAttenuation);
		onGround_ = true;
	}

	// 行列再計算と転送
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}


void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

