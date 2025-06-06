#pragma once
#include <raylib.h>

// Перечисление для типов сущностей
enum class EntityType {
    None,
    Player,
    Wolf,
    Resource,
    Generator
};

class Entity {
public:
    Entity(int x, int y);
    virtual ~Entity() = default;
    
    // Основные методы, общие для всех объектов
    virtual void Update(float dt) = 0;
    virtual void Draw(float cellSize, float marginLeft, float marginTop) const = 0;
    
    // Геттеры и сеттеры для координат
    Vector2 GetPosition() const;
    void SetPosition(int x, int y);
    
    // Работа с размерами сетки
    void SetGridSize(int width, int height);
    
    // Получение типа сущности
    EntityType GetType() const;

protected:
    int x, y;
    int gridWidth, gridHeight;
    EntityType type;
};
