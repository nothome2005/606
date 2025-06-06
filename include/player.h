#pragma once
#include "entity.h"

class Player : public Entity {
public:
    Player(int x, int y);
    void Move(int dx, int dy, int gridWidth, int gridHeight);
    void ResetTurn();
    void SetMoved(bool moved);
    bool HasMoved() const;
    void Update(float dt) override;
    void Draw(float cellSize, float marginLeft, float marginTop) const override;

private:
    bool moved;
};