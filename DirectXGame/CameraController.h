#pragma once
#include <KamataEngine.h>
//前方宣言
class Player;

struct Rect {
	float left = 0.0f;//左端
	float right = 1.0f;//右端
	float bottom = 0.0f;// 下端
	float top = 1.0f;//上端
};

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
};
