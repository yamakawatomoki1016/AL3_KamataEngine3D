#include "MyMath.h"
#include <cmath>
#include <numbers>

// 明示的に名前空間を指定して関数を呼ぶ
KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotation, const KamataEngine::Vector3& translation) {
	// KamataEngine名前空間のMathUtilityを明示
	KamataEngine::Matrix4x4 matScale = KamataEngine::MathUtility::MakeScaleMatrix(scale);
	KamataEngine::Matrix4x4 matRotX = KamataEngine::MathUtility::MakeRotateXMatrix(rotation.x);
	KamataEngine::Matrix4x4 matRotY = KamataEngine::MathUtility::MakeRotateYMatrix(rotation.y);
	KamataEngine::Matrix4x4 matRotZ = KamataEngine::MathUtility::MakeRotateZMatrix(rotation.z);

	// 回転行列の掛け算は順序に注意（問題なければそのままでOK）
	KamataEngine::Matrix4x4 matRot = matRotZ * matRotX * matRotY;

	KamataEngine::Matrix4x4 matTrans = KamataEngine::MathUtility::MakeTranslateMatrix(translation);

	KamataEngine::Matrix4x4 matWorld = matScale * matRot * matTrans;

	return matWorld;
}

// 線形補間
float Lerp(float a, float b, float t) { return a + (b - a) * t; }

float EaseInOut(float x1, float x2, float t) { 
	float easedT = -(std::cosf(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
	return Lerp(x1, x2, easedT);
}
