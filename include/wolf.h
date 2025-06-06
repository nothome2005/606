#pragma once
#include "entity.h"
#include "grid.h"
#include "player.h"

class IMoveStrategy {
public:
    virtual ~IMoveStrategy() {}
    virtual std::pair<int, int> GetNextMove(const Grid& grid, const Player& player, int x, int y) = 0;
};

class WolfMoveStrategy : public IMoveStrategy {
public:
    std::pair<int, int> GetNextMove(const Grid& grid, const Player& player, int x, int y) override;
};

class Wolf : public Entity {
public:
    Wolf(int x, int y, IMoveStrategy* strategy);
    void StartTurn(const Grid& grid, const Player& player);
    void Update(float dt) override;
    void Draw(float cellSize, float marginLeft, float marginTop) const override;
    bool IsTurnFinished() const;
private:
    int stepsPlanned;
    int dxQueue[2], dyQueue[2];
    int stepIndex;
    float moveTimer;
    bool turnFinished;
    IMoveStrategy* moveStrategy;
    bool firstTurn = true;  // Track if this is the first turn for this wolf
    void PlanSteps(const Grid& grid, const Player& player);
};