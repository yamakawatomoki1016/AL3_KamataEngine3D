#pragma once
#include "KamataEngine.h"
#include "Vector3.h"

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

private:
	//ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	//モデル
	KamataEngine::Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.05f;
	static inline const float kLimitRunSpeed = 0.5f;
	static inline const float kAttenuation = 0.5f;
	LRDirection lrDirection_ = LRDirection::kRight;
};
