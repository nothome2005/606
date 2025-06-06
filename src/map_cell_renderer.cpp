#include "map_cell_renderer.h"
#include "raylib.h"
#include "map_cell.h"
#include <unordered_map>

// Реализация методов MapCellRenderer перенесена из map_cell.cpp

extern std::unordered_map<ResourceType, float> selectionOvalYOffset;

void MapCellRenderer::Draw(const MapCell& cell, float x, float y, float size) {
    if (cell.IsSelected()) {
        float offset = 0.0f;
        auto it = selectionOvalYOffset.find(cell.GetType());
        if (it != selectionOvalYOffset.end())
            offset = it->second * size;
        DrawSelectionOval(x, y, size, offset);
    }
    if (cell.GetType() != ResourceType::None) {
        // Получаем текстуру через публичный геттер (добавим его в MapCell)
        const Texture* texture = cell.GetTexture();
        if (texture) {
            float yOffset = 0.0f;
            if (cell.GetType() == ResourceType::Tree) {
                yOffset = -size * 0.12f;
            }
            DrawTexturePro(
                *texture,
                Rectangle{0, 0, (float)texture->width, (float)texture->height},
                Rectangle{x, y + yOffset, size, size},
                Vector2{0, 0},
                0.0f,
                WHITE
            );
        }
    }
}

void MapCellRenderer::DrawSelectionOval(float x, float y, float size, float offsetY) {
    float cx = x + size / 2.0f;
    float cy = y + size / 2.0f + offsetY;
    float rx = size / 2.0f;
    float ry = size / 3.0f / 2.0f;
    int thick = 6;
    for (int i = 0; i < thick; ++i) {
        DrawEllipseLines((int)cx, (int)cy, rx - i, ry - i * 0.7f, RED);
        DrawEllipseLines((int)cx, (int)cy, rx + i, ry + i * 0.7f, RED);
    }
}

void MapCellRenderer::SetSelectionOvalYOffset(ResourceType type, float offset) {
    selectionOvalYOffset[type] = offset;
}
