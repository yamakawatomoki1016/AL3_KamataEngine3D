#pragma once
#include "KamataEngine.h"

enum class LRDirection {
	kNeutral,
	kRight,
	kLeft,
};

class Player {
public:
	/// <summary>
    /// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	/// <summary>
	/// 初期化
	/// </summary>
	void Update();

	/// <summary>
	/// 初期化
	/// </summary>
	void Draw();

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

private:
	//ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	//モデル
	KamataEngine::Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.015f;
	static inline const float kLimitRunSpeed = 0.15f;
	static inline const float kAttenuation = 0.5f;
	LRDirection lrDirection_ = LRDirection::kRight;
	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;
	//設置状態フラグ
	bool onGround_ = true;
	//重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.03f;
	//最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 1.5f;
	//ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.4f;
};
