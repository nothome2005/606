#pragma once
#include "raylib.h"
#include "map_cell.h"
#include <unordered_map>

// ¬нешн€€ таблица смещений овала выделени€ дл€ разных ресурсов
extern std::unordered_map<ResourceType, float> selectionOvalYOffset;

class MapCellRenderer {
public:
    static void Draw(const MapCell& cell, float x, float y, float size);
    static void DrawSelectionOval(float x, float y, float size, float offsetY = 0.0f);
    static void SetSelectionOvalYOffset(ResourceType type, float offset);
};
