#include "CameraController.h"
#include "Player.h"

using namespace KamataEngine::MathUtility;

void CameraController::Initialize() { 
	camera_ = new KamataEngine::Camera();
	camera_->Initialize();
}

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからの座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
	//移動範囲制限
	camera_->translation_.x = max(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = min(camera_->translation_.x, movableArea_.right);
	camera_->translation_.y = max(camera_->translation_.y, movableArea_.bottom);
	camera_->translation_.y = min(camera_->translation_.y, movableArea_.top);
	//行列を更新する
	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	//追従対象のワールドトランスフォームを参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	//追従対象とオフセットからの座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}
