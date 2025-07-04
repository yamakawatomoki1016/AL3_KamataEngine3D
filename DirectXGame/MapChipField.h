#pragma once
#include <cstdint> 
#include <stdint.h> 
#include <vector>
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include "KamataEngine.h"
#include "Rect.h"

using Common::Rect;

enum class MapChipType {
	kBlank,
	kBlock,
};
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};
struct IndexSet {
	uint32_t xIndex;
	uint32_t yIndex;
};

class MapChipField {
public:
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	//1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	//ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;
	MapChipData mapChipData_;
};
