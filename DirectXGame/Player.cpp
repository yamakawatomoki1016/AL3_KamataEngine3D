#include "Player.h"
#include "MyMath.h" 

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera) {
	//NULLポインタチェック
	assert(model);
	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;
	camera_ = camera;
	//ワールド変換の初期化
	worldTransform_.Initialize();
}

void Player::Update() {
    // アフィン変換行列を計算してメンバ変数に代入
    worldTransform_.matWorld_ = MakeAffineMatrix(
        worldTransform_.scale_,
        worldTransform_.rotation_,
        worldTransform_.translation_
    );

    // 定数バッファに転送
    worldTransform_.TransferMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }
