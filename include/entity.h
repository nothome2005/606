#pragma once
#include <raylib.h>

// ������������ ��� ����� ���������
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
    
    // �������� ������, ����� ��� ���� ��������
    virtual void Update(float dt) = 0;
    virtual void Draw(float cellSize, float marginLeft, float marginTop) const = 0;
    
    // ������� � ������� ��� ���������
    Vector2 GetPosition() const;
    void SetPosition(int x, int y);
    
    // ������ � ��������� �����
    void SetGridSize(int width, int height);
    
    // ��������� ���� ��������
    EntityType GetType() const;

protected:
    int x, y;
    int gridWidth, gridHeight;
    EntityType type;
};
