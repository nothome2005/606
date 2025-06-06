#include "player.h"
#include "resource_manager.h"

Player::Player(int x, int y)
    : Entity(x, y), moved(false) {
    type = EntityType::Player;
}

void Player::Move(int dx, int dy, int gridWidth, int gridHeight) {
    int newX = x + dx;
    int newY = y + dy;
    if (newX >= 0 && newX < gridWidth && newY >= 0 && newY < gridHeight) {
        // ѕровер€ем, не пытаемс€ ли мы войти в зону генератора
        if (!((newX == 4 || newX == 5) && (newY == 4 || newY == 5))) {
            x = newX;
            y = newY;
            moved = true;
        }
    }
}

void Player::ResetTurn() {
    moved = false;
}

void Player::SetMoved(bool moved) {
    this->moved = moved;
}

bool Player::HasMoved() const {
    return moved;
}

void Player::Update(float dt) {
    if (moved) return;
    int dx = 0, dy = 0;
    if (IsKeyPressed(KEY_UP)) dy = -1;
    else if (IsKeyPressed(KEY_DOWN)) dy = 1;
    else if (IsKeyPressed(KEY_LEFT)) dx = -1;
    else if (IsKeyPressed(KEY_RIGHT)) dx = 1;
    if (dx != 0 || dy != 0) Move(dx, dy, gridWidth, gridHeight);
}

void Player::Draw(float cellSize, float marginLeft, float marginTop) const {
    float px = marginLeft + x * cellSize;
    float py = marginTop + y * cellSize;
    float scaleX = cellSize / (float)player_idle_texture.width;
    float scaleY = cellSize / (float)player_idle_texture.height;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    float texW = player_idle_texture.width * scale;
    float texH = player_idle_texture.height * scale;
    float offsetX = px + (cellSize - texW) / 2.0f;
    float offsetY = py + (cellSize - texH);
    DrawTextureEx(player_idle_texture, {offsetX, offsetY}, 0, scale, WHITE);
}
