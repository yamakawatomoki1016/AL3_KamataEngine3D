#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() { 
	textureHandle_ = TextureManager::Load("mario.jpg"); 
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	model_ = Model::Create();
	//ワールドトランスフォーム
	worldTransform_.Initialize();
	//カメラの初期化
	camera_.Initialize();
	// サウンドデータハンドル
	soundDataHandle_ = Audio::GetInstance()->LoadWave("fanfare.wav");
	// 音声再生
	//Audio::GetInstance()->PlayWave(soundDataHandle_);
	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true);
	//ライン描画が参照するかめらを指定する
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);
	//デバッグカメラ生成
	debugCamera_ = new DebugCamera(1280, 720);
	//軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
}

void GameScene::Update() {
	// スプライトの今の座標を取得
	Vector2 position = sprite_->GetPosition();
	//座標を｛２，１｝移動
	position.x += 2.0f;
	position.y += 1.0f;
	//移動した座標をスプライトに反映
	sprite_->SetPosition(position);
	//スペースキーを押した瞬間
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		//音声停止
		Audio::GetInstance()->StopWave(voiceHandle_);
	}
	//デバッグテキストの表示
#ifdef _DEBUG
	//float3入力ボックス
	ImGui::InputFloat3("inputFloat3", inputFloat3);
	//float3スライダー
	ImGui::SliderFloat3("SliderFloat3", inputFloat3, 0.0f, 1.0f);
	//デモウィンドウの表示の有効化
	ImGui::ShowDemoWindow();
#endif
	debugCamera_->Update();
}

void GameScene::Draw() { 
	//DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance(); 

	//スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	//sprite_->Draw();

	//スプライト描画後処理
	Sprite::PostDraw();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画
	//model_->Draw(worldTransform_, camera_, textureHandle_);
	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);

	// 3Dモデル描画後処理
	Model::PostDraw();

	//ラインを描画する
	//PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
}

GameScene::~GameScene() { 
	delete sprite_; 
	delete model_;
	delete debugCamera_;
}
