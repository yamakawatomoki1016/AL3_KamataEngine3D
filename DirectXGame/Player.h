#pragma once
#include "KamataEngine.h"

class MapChipField;

enum class LRDirection {
	kNeutral,
	kRight,
	kLeft,
};

struct CollisionMapInfo {
	bool isHitCeiling_ = false;//天井衝突フラグ
	bool isOnGround_ = false;//着地フラグ
	bool isHitWall_ = false;//壁接触フラグ	
	KamataEngine::Vector3 moveAmount_;//移動量
};	
	
enum Corner {	
	KrightBottom,//	右下
	kLeftBottom, //	左下
	kRightTop,	 // 右上
	kLeftTop,	 // 左上
	kNumCorner	 // 要素数
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

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void InputMove();

	void CheckMapCollision(CollisionMapInfo& info);

	bool CheckCollisionUp(CollisionMapInfo& info);
	/*bool CheckCollisionDown(const CollisionMapInfo& info);
	bool CheckCollisionLeft(const CollisionMapInfo& info);
	bool CheckCollisionRight(const CollisionMapInfo& info);*/
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);
	void ApplyCollisionResult(const CollisionMapInfo& info);
	void ResolveCeilingCollision(const CollisionMapInfo& info);

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
	static inline const float kGravityAcceleration = 0.02f;
	//最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 1.5f;
	//ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.4f;
	//マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
	//キャラクターの当たり判定サイズ
	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;
	static inline const float kBlank = 0.005f;
};
