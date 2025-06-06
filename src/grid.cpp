#include "grid.h"
#include "raylib.h"
#include "map_cell_renderer.h"

static const Color DARK_BLUE_CELL = { 20, 30, 60, 255 };

Grid::Grid(int rows, int cols, float marginLeft, float marginTop, float cellSize)
    : rows(rows), cols(cols), marginLeft(marginLeft), marginTop(marginTop), cellSize(cellSize)
{
    buttons.reserve(rows * cols);
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            float bx = marginLeft + x * cellSize;
            float by = marginTop + y * cellSize;
            buttons.emplace_back(bx, by, cellSize, cellSize);
        }
    }
    mapCells.resize(rows * cols);
}

void Grid::SetMapCells(const std::vector<MapCell>& cells) {
    mapCells = cells;
}

void Grid::SetGenerator(Generator* gen) {
    generator = gen;
}

void Grid::Draw() const {
    static const Color FROSTY_DARK_BLUE_TRANSPARENT = { 30, 50, 80, 120 };
    for (int y = 0; y <= rows; ++y) {
        float yPos = marginTop + y * cellSize;
        DrawLineEx(
            { marginLeft, yPos },
            { marginLeft + cols * cellSize, yPos },
            2.0f, FROSTY_DARK_BLUE_TRANSPARENT
        );
    }
    for (int x = 0; x <= cols; ++x) {
        float xPos = marginLeft + x * cellSize;
        DrawLineEx(
            { xPos, marginTop },
            { xPos, marginTop + rows * cellSize },
            2.0f, FROSTY_DARK_BLUE_TRANSPARENT
        );
    }

    for (size_t i = 0; i < buttons.size(); ++i) {
        int gx = i % cols, gy = i / cols;
        bool isCenter = (gx == 4 || gx == 5) && (gy == 4 || gy == 5);
        if (i < mapCells.size() && !(generator && isCenter))
            MapCellRenderer::Draw(mapCells[i], buttons[i].rect.x, buttons[i].rect.y, buttons[i].rect.width);
    }
    
    // Вызываем Draw с передачей параметров
    if (generator) generator->Draw(cellSize, marginLeft, marginTop);
}

void Grid::Update() {
    // Вызываем Update с передачей параметра dt
    if (generator) generator->Update(GetFrameTime());
}