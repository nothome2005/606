#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>
#include <memory>

// Предварительное объявление класса Resource
class Resource;

enum class ResourceType {
    None,
    Tree,
    Coal,
    Stone
};

class MapCell {
public:
    MapCell();
    MapCell(ResourceType type, int mass, Texture* texture);
    ~MapCell() = default;

    ResourceType GetType() const;
    int GetMass() const;
    void Set(ResourceType type, int mass, Texture* texture);

    void SetSelected(bool sel) { selected = sel; }
    bool IsSelected() const { return selected; }
    bool IsClicked(float x, float y, float size) const;
    const Texture* GetTexture() const { return texture; }
    
    // Новые методы для работы с ресурсом как сущностью
    void SetResource(std::shared_ptr<Resource> res);
    std::shared_ptr<Resource> GetResource() const;
    bool HasResource() const;

private:
    ResourceType type;
    int mass; // tons
    Texture* texture;
    bool selected = false;
    std::shared_ptr<Resource> resource;
};