#include "resource_manager.h"
#include <string>
#include <cstring>

Texture g_background = { 0 };
Texture coal_ = { 0 };
Texture iron_ = { 0 };
Texture tree_ = { 0 };
Texture generator_ = { 0 };
Texture cursor_ = { 0 };
Texture player_idle_texture = { 0 };
Texture wolf_texture = { 0 };

void LoadResources()
{
    g_background = LoadTexture("background2-fs8.png");
    coal_ = LoadTexture("coal-fs8.png");
    iron_ = LoadTexture("iron-fs8.png");
    tree_ = LoadTexture("tree-fs8.png");
    generator_ = LoadTexture("generator3-fs8.png");
    cursor_ = LoadTexture("cursor-fs8.png");
    player_idle_texture = LoadTexture("Player_IDLE-fs8.png");
    wolf_texture = LoadTexture("wolf-fs8.png");
}

void UnloadResources()
{
    UnloadTexture(g_background);
    UnloadTexture(coal_);
    UnloadTexture(iron_);
    UnloadTexture(tree_);
    UnloadTexture(generator_);
    UnloadTexture(cursor_);
    UnloadTexture(player_idle_texture);
    UnloadTexture(wolf_texture);
}