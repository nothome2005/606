#pragma once
#include "raylib.h"

// Глобальный доступ к текстурам (и другим ресурсам)
extern Texture g_background;
extern Texture coal_;
extern Texture iron_;
extern Texture tree_;
extern Texture generator_;
extern Texture cursor_;
extern Texture player_idle_texture;
extern Texture wolf_texture;

// Загрузка всех ресурсов
void LoadResources();

// Выгрузка всех ресурсов
void UnloadResources();