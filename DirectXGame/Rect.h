#pragma once

namespace Common {

struct Rect {
	float left = 0.0f;
	float right = 1.0f;
	float bottom = 0.0f;
	float top = 1.0f;

	Rect() = default;
	Rect(float l, float r, float b, float t) : left(l), right(r), bottom(b), top(t) {}
};

} // namespace Common
