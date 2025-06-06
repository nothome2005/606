#include "entity.h"

Entity::Entity(int x, int y) 
    : x(x), y(y), gridWidth(10), gridHeight(10), type(EntityType::None) 
{
}

Vector2 Entity::GetPosition() const 
{
    return { (float)x, (float)y };
}

void Entity::SetPosition(int x, int y) 
{
    this->x = x;
    this->y = y;
}

void Entity::SetGridSize(int width, int height) 
{
    gridWidth = width;
    gridHeight = height;
}

EntityType Entity::GetType() const 
{
    return type;
}