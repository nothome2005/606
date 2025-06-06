#pragma once
#include "raylib.h"
#include "entity.h"
#include <vector>

// --- Эффект дыма без текстур (через примитивы) ---
struct SmokeParticle {
    Vector2 pos;
    float radius;
    float alpha;
    float vy;
    float grow;
    float life;
};

extern float generatorSelectionOvalYOffset;

// Генератор занимает 4 клетки: (4,4), (4,5), (5,4), (5,5) на 10x10
class Generator : public Entity {
public:
    // Конструкторы для разных случаев использования
    Generator(int gridX, int gridY, float cellSize, float marginLeft, float marginTop);
    Generator(float cellSize, float marginLeft, float marginTop); // Для обратной совместимости

    void Update(float dt) override;
    void Draw(float cellSize, float marginLeft, float marginTop) const override;
    
    // Для обратной совместимости
    void Update() { Update(GetFrameTime()); }
    void Draw() const { Draw(this->cellSize, this->marginLeft, this->marginTop); }

    // --- Выделение ---
    void SetSelected(bool sel) { selected = sel; }
    bool IsSelected() const { return selected; }
    // Проверка клика по генератору (центральные 4 клетки)
    bool IsClicked(float mouseX, float mouseY) const;

    // Овал выделения для генератора с возможностью смещения по Y
    static void DrawSelectionOval(float x, float y, float width, float height, float offsetY = 0.0f);

    // Управление смещением
    static void SetSelectionOvalYOffset(float offset);

private:
    float cellSize, marginLeft, marginTop;

    // --- Дым ---
    mutable std::vector<SmokeParticle> smokeParticles;
    float smokeTimer = 0.0f;
    void UpdateSmoke();
    void DrawSmoke() const;

    // --- Светлячок (только снизу генератора) ---
    float fireflyTimer = 0.0f;
    float fireflyAlpha = 0.7f;
    float fireflyAlphaDir = 1.0f; // 1.0 - ярче, -1.0 - тусклее
    void UpdateFirefly();
    void DrawFireflyGlow() const;

    // --- Выделение ---
    bool selected = false;
};