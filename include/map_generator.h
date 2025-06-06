#pragma once
#include "map_cell.h"
#include <vector>

class MapGenerator {
public:
    // rows, cols - размер сетки, mass - масса ресурса в клетке
    static std::vector<MapCell> Generate(int rows, int cols, int treeCount, int coalCount, int stoneCount, int massPerCell);
};