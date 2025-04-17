#include "Player.h"

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle) { 
	//NULLポインタチェック
	assert(model);
	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle = textureHandle_;
	//ワールド変換の初期化
	worldTransform_.Initialize();
}

void Player::Update() {}

void Player::Draw() {}
