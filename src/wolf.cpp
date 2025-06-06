#include "wolf.h"
#include "resource_manager.h"
#include <queue>

// --- WolfMoveStrategy реализаци€ ---
std::pair<int, int> WolfMoveStrategy::GetNextMove(const Grid& grid, const Player& player, int x, int y) {
    int px = (int)player.GetPosition().x;
    int py = (int)player.GetPosition().y;
    int gridW = grid.GetCols();
    int gridH = grid.GetRows();
    const auto& cells = grid.GetMapCells();
    // Y приоритет
    if (y < py) {
        int ny = y + 1;
        if (ny < gridH && cells[ny * gridW + x].GetType() == ResourceType::None && !((x == 4 || x == 5) && (ny == 4 || ny == 5)))
            return {0, 1};
    } else if (y > py) {
        int ny = y - 1;
        if (ny >= 0 && cells[ny * gridW + x].GetType() == ResourceType::None && !((x == 4 || x == 5) && (ny == 4 || ny == 5)))
            return {0, -1};
    }
    // X приоритет
    if (x < px) {
        int nx = x + 1;
        if (nx < gridW && cells[y * gridW + nx].GetType() == ResourceType::None && !((nx == 4 || nx == 5) && (y == 4 || y == 5)))
            return {1, 0};
    } else if (x > px) {
        int nx = x - 1;
        if (nx >= 0 && cells[y * gridW + nx].GetType() == ResourceType::None && !((nx == 4 || nx == 5) && (y == 4 || y == 5)))
            return {-1, 0};
    }
    return {0, 0};
}

Wolf::Wolf(int x, int y, IMoveStrategy* strategy)
    : Entity(x, y), stepsPlanned(0), stepIndex(0), moveTimer(0), turnFinished(false), 
      moveStrategy(strategy), firstTurn(true) {
    type = EntityType::Wolf;
}

void Wolf::StartTurn(const Grid& grid, const Player& player) {
    stepIndex = 0;
    moveTimer = 0;
    
    // ≈сли это первый ход волка, то просто пропускаем ход (не планируем движение)
    if (firstTurn) {
        firstTurn = false;
        turnFinished = true;
        return;
    }
    
    // »наче планируем шаги как обычно
    PlanSteps(grid, player);
    turnFinished = (stepsPlanned == 0);  // ≈сли нет шагов, сразу отмечаем ход как завершенный
}

void Wolf::Update(float dt) {
    if (turnFinished) return;
    
    moveTimer += dt;
    
    // Increased delay between wolf movements for better gameplay pacing
    if (moveTimer >= 0.75f && stepIndex < stepsPlanned) {
        int newX = x + dxQueue[stepIndex];
        int newY = y + dyQueue[stepIndex];
        
        // Ensure the wolf stays within grid bounds
        if (newX >= 0 && newX < gridWidth && newY >= 0 && newY < gridHeight) {
            x = newX;
            y = newY;
        }
        
        moveTimer = 0;
        ++stepIndex;
        if (stepIndex >= stepsPlanned) {
            turnFinished = true;
        }
    }
}

bool Wolf::IsTurnFinished() const {
    return turnFinished;
}

void Wolf::PlanSteps(const Grid& grid, const Player& player) {
    int cx = x, cy = y;
    stepsPlanned = 0;
    
    // Plan up to 2 steps
    for (int s = 0; s < 2; ++s) {
        auto move = moveStrategy->GetNextMove(grid, player, cx, cy);
        int dx = move.first, dy = move.second;
        if (dx != 0 || dy != 0) {
            int newX = cx + dx;
            int newY = cy + dy;
            
            // Make sure wolf stays within grid boundaries
            if (newX >= 0 && newX < gridWidth && newY >= 0 && newY < gridHeight) {
                dxQueue[s] = dx;
                dyQueue[s] = dy;
                cx = newX;
                cy = newY;
                ++stepsPlanned;
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

void Wolf::Draw(float cellSize, float marginLeft, float marginTop) const {
    float px = marginLeft + x * cellSize;
    float py = marginTop + y * cellSize;
    float scaleX = cellSize / (float)wolf_texture.width;
    float scaleY = cellSize / (float)wolf_texture.height;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    float texW = wolf_texture.width * scale;
    float texH = wolf_texture.height * scale;
    float offsetX = px + (cellSize - texW) / 2.0f;
    float offsetY = py + (cellSize - texH);
    DrawTextureEx(wolf_texture, {offsetX, offsetY}, 0, scale, WHITE);
}
