#pragma once
#include <KamataEngine.h>
#include "Rect.h"
//前方宣言
class Player;

using Common::Rect;

class CameraController {
public:

	void Initialize();

	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	const KamataEngine::Camera* GetViewProjection() const { return camera_; }

	void SetMovableArea(Rect area) { movableArea_ = area; }

private:
	//カメラ
	KamataEngine::Camera* camera_;
	Player* target_ = nullptr;
	//追従対象とカメラの座標の差（オフセット）
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};
	Rect movableArea_ = {0, 100, 0, 100};
	//カメラの目標座標
	KamataEngine::Vector3 targetPosition_;
	static inline const float kInterpolationRate = 0.1f;
	//速度掛け率
	static inline const float kVelocityBias = 20.0f;
	static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};
};
