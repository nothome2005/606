#pragma once
#include "grid.h"
#include <vector>
#include "raylib.h"
#include "resource_manager.h"
#include "map_cell.h"
#include <string>
#include <fstream>
#include <filesystem>
#include "button.h"
#include <memory>

// Типы объектов редактора
enum class EditorObjectType {
    None,
    Tree,
    Coal,
    Stone,
    Player,
    Wolf,
    Generator
};

extern bool g_EditorBackToMenu;

class LevelCreator {
public:
    LevelCreator();
    void Update();
    void PlaceResource(int key);
    void ResizeGrid(int newSize);
    bool SaveToFile(const char* filename) const;
    // Геттеры для UI
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    int getCurrentX() const { return currentX; }
    int getCurrentY() const { return currentY; }
    float getMarginLeft() const { return marginLeft; }
    float getMarginTop() const { return marginTop; }
    float getCellSize() const { return cellSize; }
    const char* getMapName() const { return mapName; }
    void setMapNameChar(int idx, char c) { mapName[idx] = c; }
    void setMapNameEnd(int idx) { mapName[idx] = 0; }
    void setMapName(const char* src);
    const std::vector<MapCell>& getMapCells() const { return mapCells; }
    const std::vector<EditorObjectType>& getObjectTypes() const { return objectTypes; }
    Grid& getGrid() { return grid; }
    void clearCell(int idx);
private:
    int currentX, currentY;
    int rows, cols;
    float marginLeft, marginTop, cellSize;
    std::vector<MapCell> mapCells;
    std::vector<EditorObjectType> objectTypes;
    Grid grid;
    char mapName[32] = "";
    void SyncGrid();
};

// UI класс
class LevelCreatorUI {
public:
    LevelCreatorUI(LevelCreator& model);
    void Update();
    void Draw() const;
private:
    LevelCreator& model;
    bool nameEditActive = false;
    Rectangle nameEditRect = { 50, 50, 300, 40 };
    std::unique_ptr<Button> btnInc, btnDec, btnSave, btnBack;
    void DrawUI() const;
    void HandleUI();
    void OnInc();
    void OnDec();
    void OnSave();
    void OnBack();
};
