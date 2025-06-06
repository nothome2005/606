#include "LevelCreator.h"
#include "map_cell.h"
#include "map_cell_renderer.h"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <memory>

bool g_EditorBackToMenu = false;

LevelCreator::LevelCreator()
    : rows(10), cols(10), marginLeft(50.0f), marginTop(150.0f), cellSize((1024.0f - 50.0f - 250.0f) / 10),
      currentX(0), currentY(0),
      mapCells(rows * cols),
      objectTypes(rows * cols, EditorObjectType::None),
      grid(rows, cols, marginLeft, marginTop, cellSize)
{
    SyncGrid();
}

void LevelCreator::SyncGrid() {
    grid.SetMapCells(mapCells);
}

void LevelCreator::Update() {
    // Исправлено направление перемещения: X - горизонталь (столбцы), Y - вертикаль (строки)
    if (IsKeyPressed(KEY_UP))    currentY = std::max(0, currentY - 1);
    if (IsKeyPressed(KEY_DOWN))  currentY = std::min(rows - 1, currentY + 1);
    if (IsKeyPressed(KEY_LEFT))  currentX = std::max(0, currentX - 1);
    if (IsKeyPressed(KEY_RIGHT)) currentX = std::min(cols - 1, currentX + 1);
    currentX = std::max(0, std::min(currentX, cols - 1));
    currentY = std::max(0, std::min(currentY, rows - 1));
    for (int k = KEY_ONE; k <= KEY_SIX; ++k) {
        if (IsKeyPressed((KeyboardKey)k)) PlaceResource(k);
    }
    if (IsKeyPressed(KEY_ZERO)) {
        int idx = currentY * cols + currentX;
        clearCell(idx);
    }
}

void LevelCreator::clearCell(int idx) {
    if (idx >= 0 && idx < (int)objectTypes.size()) {
        if (objectTypes[idx] == EditorObjectType::Generator) {
            int gx = currentX, gy = currentY;
            for (int dy = 0; dy <= 1; ++dy) for (int dx = 0; dx <= 1; ++dx) {
                int tx = currentX - dx, ty = currentY - dy;
                if (tx >= 0 && ty >= 0 && tx < cols-1 && ty < rows-1) {
                    bool isGen = true;
                    for (int sy = 0; sy < 2; ++sy)
                        for (int sx = 0; sx < 2; ++sx)
                            if (objectTypes[(ty+sy)*cols + (tx+sx)] != EditorObjectType::Generator)
                                isGen = false;
                    if (isGen) { gx = tx; gy = ty; }
                }
            }
            for (int dy = 0; dy < 2; ++dy)
                for (int dx = 0; dx < 2; ++dx)
                    objectTypes[(gy+dy)*cols + (gx+dx)] = EditorObjectType::None;
        } else {
            objectTypes[idx] = EditorObjectType::None;
        }
        mapCells[idx].Set(ResourceType::None, 0, nullptr);
        SyncGrid();
    }
}

void LevelCreator::PlaceResource(int key) {
    int idx = currentY * cols + currentX;
    if (idx < 0 || idx >= (int)objectTypes.size()) return;
    ResourceType type = ResourceType::None;
    Texture* tex = nullptr;
    EditorObjectType objType = EditorObjectType::None;
    switch (key) {
        case KEY_ONE:   type = ResourceType::Tree;   tex = &tree_;      objType = EditorObjectType::Tree; break;
        case KEY_TWO:   type = ResourceType::Coal;   tex = &coal_;      objType = EditorObjectType::Coal; break;
        case KEY_THREE: type = ResourceType::Stone;  tex = &iron_;      objType = EditorObjectType::Stone; break;
        case KEY_FOUR:  type = ResourceType::None;   tex = &player_idle_texture; objType = EditorObjectType::Player; break;
        case KEY_FIVE:  type = ResourceType::None;   tex = &wolf_texture; objType = EditorObjectType::Wolf; break;
        case KEY_SIX:   type = ResourceType::None;   tex = &generator_; objType = EditorObjectType::Generator; break;
    }
    if (objType == EditorObjectType::Generator) {
        if (currentX < cols-1 && currentY < rows-1) {
            bool canPlace = true;
            for (int dy = 0; dy < 2; ++dy)
                for (int dx = 0; dx < 2; ++dx)
                    if (objectTypes[(currentY+dy)*cols + (currentX+dx)] == EditorObjectType::Generator)
                        canPlace = false;
            if (currentX > 0 && objectTypes[currentY*cols + (currentX-1)] == EditorObjectType::Generator) canPlace = false;
            if (currentY > 0 && objectTypes[(currentY-1)*cols + currentX] == EditorObjectType::Generator) canPlace = false;
            if (!canPlace) return;
            for (int dy = 0; dy < 2; ++dy)
                for (int dx = 0; dx < 2; ++dx)
                    objectTypes[(currentY+dy)*cols + (currentX+dx)] = EditorObjectType::Generator;
        }
    } else if (objType == EditorObjectType::Player) {
        for (auto& t : objectTypes) if (t == EditorObjectType::Player) t = EditorObjectType::None;
        objectTypes[idx] = EditorObjectType::Player;
    } else {
        objectTypes[idx] = objType;
    }
    if (objType == EditorObjectType::Tree || objType == EditorObjectType::Coal || objType == EditorObjectType::Stone)
        mapCells[idx].Set(type, 1, tex);
    else
        mapCells[idx].Set(ResourceType::None, 0, nullptr);
    SyncGrid();
}

void LevelCreator::setMapName(const char* src) {
    strncpy(mapName, src, 31);
    mapName[31] = 0;
}

bool LevelCreator::SaveToFile(const char* filename) const {
    std::ofstream out(filename);
    if (!out) return false;
    out << rows << ' ' << cols << '\n';
    out << mapName << '\n';
    for (int i = 0; i < rows * cols; ++i) {
        out << (int)objectTypes[i] << ' ';
    }
    out << '\n';
    for (int i = 0; i < rows * cols; ++i) {
        out << (int)mapCells[i].GetType() << ' ' << mapCells[i].GetMass() << ' ';
    }
    out << '\n';
    return true;
}

void LevelCreator::ResizeGrid(int newSize) {
    rows = newSize;
    cols = newSize;
    cellSize = std::min((1024.0f - marginLeft - 50.0f) / cols, (1024.0f - marginTop - 50.0f) / rows);
    mapCells = std::vector<MapCell>(rows * cols);
    objectTypes = std::vector<EditorObjectType>(rows * cols, EditorObjectType::None);
    grid = Grid(rows, cols, marginLeft, marginTop, cellSize);
    currentX = 0;
    currentY = 0;
    SyncGrid();
}

// --- LevelCreatorUI ---
LevelCreatorUI::LevelCreatorUI(LevelCreator& model)
    : model(model)
{
    btnInc = std::make_unique<Button>(360, 100, 50, 40, "+", 32);
    btnDec = std::make_unique<Button>(420, 100, 50, 40, "-", 32);
    btnSave = std::make_unique<Button>(360, 50, 100, 40, "Save", 32);
    btnBack = std::make_unique<Button>(470, 50, 100, 40, "Back", 32);
    btnInc->SetOnClick([this]() { OnInc(); });
    btnDec->SetOnClick([this]() { OnDec(); });
    btnSave->SetOnClick([this]() { OnSave(); });
    btnBack->SetOnClick([this]() { OnBack(); });
}

void LevelCreatorUI::OnInc() {
    int newSize = std::min(20, model.getCols() + 1);
    model.ResizeGrid(newSize);
}
void LevelCreatorUI::OnDec() {
    int newSize = std::max(5, model.getCols() - 1);
    model.ResizeGrid(newSize);
}
void LevelCreatorUI::OnSave() {
    std::filesystem::create_directory("maps");
    std::string fname = std::string("maps/") + (model.getMapName()[0] ? model.getMapName() : "unnamed") + ".lvl";
    model.SaveToFile(fname.c_str());
}
void LevelCreatorUI::OnBack() {
    g_EditorBackToMenu = true;
}

void LevelCreatorUI::Update() {
    HandleUI();
    btnInc->Update();
    btnDec->Update();
    btnSave->Update();
    btnBack->Update();
}

void LevelCreatorUI::Draw() const {
    // 1. Нарисовать сетку (фон клеток и линии)
    const int rows = model.getRows();
    const int cols = model.getCols();
    const float marginLeft = model.getMarginLeft();
    const float marginTop = model.getMarginTop();
    const float cellSize = model.getCellSize();
    const auto& mapCells = model.getMapCells();
    const auto& objectTypes = model.getObjectTypes();
    int currentX = model.getCurrentX();
    int currentY = model.getCurrentY();
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int idx = y * cols + x;
            float cx = marginLeft + x * cellSize;
            float cy = marginTop + y * cellSize;
            Color cellColor = (x == currentX && y == currentY) ? WHITE : Color{30, 50, 80, 120};
            DrawRectangle(cx, cy, cellSize, cellSize, Fade(cellColor, (x == currentX && y == currentY) ? 0.10f : 0.18f));
            DrawRectangleLines(cx, cy, cellSize, cellSize, (x == currentX && y == currentY) ? WHITE : Color{30, 50, 80, 180});
            if (idx >= 0 && idx < (int)mapCells.size())
                MapCellRenderer::Draw(mapCells[idx], cx, cy, cellSize);
        }
    }
    // --- Игрок ---
    bool playerDrawn = false;
    for (int y = 0; y < rows && !playerDrawn; ++y) {
        for (int x = 0; x < cols && !playerDrawn; ++x) {
            int idx = y * cols + x;
            if (idx >= 0 && idx < (int)objectTypes.size() && objectTypes[idx] == EditorObjectType::Player) {
                float cx = marginLeft + x * cellSize;
                float cy = marginTop + y * cellSize;
                float scale = std::min(cellSize / (float)player_idle_texture.width, cellSize / (float)player_idle_texture.height);
                float texW = player_idle_texture.width * scale;
                float texH = player_idle_texture.height * scale;
                float offsetX = cx + (cellSize - texW) / 2.0f;
                float offsetY = cy + (cellSize - texH) / 2.0f;
                DrawTextureEx(player_idle_texture, {offsetX, offsetY}, 0, scale, WHITE);
                playerDrawn = true;
            }
        }
    }
    // --- Генератор (2x2, одна большая иконка) ---
    for (int y = 0; y < rows-1; ++y) {
        for (int x = 0; x < cols-1; ++x) {
            int idx = y * cols + x;
            if (idx >= 0 && idx < (int)objectTypes.size() && objectTypes[idx] == EditorObjectType::Generator) {
                bool isGen = true;
                for (int dy = 0; dy < 2; ++dy)
                    for (int dx = 0; dx < 2; ++dx)
                        if (objectTypes[(y+dy)*cols + (x+dx)] != EditorObjectType::Generator) isGen = false;
                if (!isGen) continue;
                float cx = marginLeft + x * cellSize;
                float cy = marginTop + y * cellSize;
                float size = cellSize * 2.0f;
                float scale = size / std::max((float)generator_.width, (float)generator_.height);
                float texW = generator_.width * scale;
                float texH = generator_.height * scale;
                float offsetX = cx + (size - texW) / 2.0f;
                float offsetY = cy + (size - texH) / 2.0f;
                DrawTextureEx(generator_, {offsetX, offsetY}, 0, scale, WHITE);
            }
        }
    }
    // --- Волк ---
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int idx = y * cols + x;
            if (idx >= 0 && idx < (int)objectTypes.size() && objectTypes[idx] == EditorObjectType::Wolf) {
                float cx = marginLeft + x * cellSize;
                float cy = marginTop + y * cellSize;
                float scaleX = cellSize / (float)wolf_texture.width;
                float scaleY = cellSize / (float)wolf_texture.height;
                float scale = std::min(scaleX, scaleY);
                float texW = wolf_texture.width * scale;
                float texH = wolf_texture.height * scale;
                float offsetX = cx + (cellSize - texW) / 2.0f;
                float offsetY = cy + (cellSize - texH) / 2.0f;
                DrawTextureEx(wolf_texture, {offsetX, offsetY}, 0, scale, WHITE);
            }
        }
    }
    // 2. Нарисовать линии сетки поверх
    for (int y = 0; y <= rows; ++y) {
        float yPos = marginTop + y * cellSize;
        DrawLineEx({marginLeft, yPos}, {marginLeft + cols * cellSize, yPos}, 2.0f, Color{30, 50, 80, 180});
    }
    for (int x = 0; x <= cols; ++x) {
        float xPos = marginLeft + x * cellSize;
        DrawLineEx({xPos, marginTop}, {xPos, marginTop + rows * cellSize}, 2.0f, Color{30, 50, 80, 180});
    }
    // 3. Инвентарь/подсказка
    float invY = 900;
    float invX = 100;
    float iconSize = 64;
    float gap = 24;
    struct InvItem { Texture* tex; int num; } items[] = {
        {&tree_, 1}, {&coal_, 2}, {&iron_, 3}, {&player_idle_texture, 4}, {&wolf_texture, 5}, {&generator_, 6}
    };
    for (int i = 0; i < 6; ++i) {
        float x = invX + i * (iconSize + gap);
        DrawRectangle(x, invY, iconSize, iconSize, Fade(WHITE, 0.15f));
        DrawRectangleLines(x, invY, iconSize, iconSize, WHITE);
        float scale = std::min(iconSize / (float)items[i].tex->width, iconSize / (float)items[i].tex->height);
        float texW = items[i].tex->width * scale;
        float texH = items[i].tex->height * scale;
        float offsetX = x + (iconSize - texW) / 2.0f;
        float offsetY = invY + (iconSize - texH) / 2.0f;
        DrawTextureEx(*items[i].tex, {offsetX, offsetY}, 0, scale, WHITE);
        DrawText(TextFormat("%d", items[i].num), x + iconSize - 24, invY + iconSize - 32, 32, WHITE);
    }
    // 4. UI редактора
    DrawUI();
}

void LevelCreatorUI::DrawUI() const {
    DrawRectangleRec(nameEditRect, nameEditActive ? Fade(WHITE, 0.25f) : Fade(WHITE, 0.12f));
    DrawRectangleLinesEx(nameEditRect, 2, nameEditActive ? YELLOW : GRAY);
    DrawText("Map name:", nameEditRect.x, nameEditRect.y - 32, 24, WHITE);
    DrawText(model.getMapName(), nameEditRect.x + 8, nameEditRect.y + 8, 28, WHITE);
    btnInc->Draw();
    btnDec->Draw();
    btnSave->Draw();
    btnBack->Draw();
    DrawText(TextFormat("Size: %dx%d", model.getCols(), model.getRows()), 480, 110, 24, WHITE);
}

void LevelCreatorUI::HandleUI() {
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, nameEditRect)) {
            nameEditActive = true;
        } else {
            nameEditActive = false;
        }
    }
    if (nameEditActive) {
        int key = GetCharPressed();
        while (key > 0) {
            int len = (int)strlen(model.getMapName());
            if ((key >= 32) && (key <= 125) && len < 31) {
                model.setMapNameChar(len, (char)key);
                model.setMapNameEnd(len+1);
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = (int)strlen(model.getMapName());
            if (len > 0) model.setMapNameEnd(len-1);
        }
    }
}