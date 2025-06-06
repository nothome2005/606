#include "map_cell.h"
#include "resource.h"
#include <unordered_map>

// Define the global variable needed by map_cell_renderer.cpp
std::unordered_map<ResourceType, float> selectionOvalYOffset = {
    {ResourceType::Tree, 0.2f},
    {ResourceType::Coal, 0.4f},
    {ResourceType::Stone, 0.4f}
};

MapCell::MapCell()
    : type(ResourceType::None), mass(0), texture(nullptr), selected(false), resource(nullptr)
{
}

MapCell::MapCell(ResourceType type, int mass, Texture* texture)
    : type(type), mass(mass), texture(texture), selected(false), resource(nullptr)
{
}

ResourceType MapCell::GetType() const {
    return type;
}

int MapCell::GetMass() const {
    return mass;
}

void MapCell::Set(ResourceType type, int mass, Texture* texture) {
    this->type = type;
    this->mass = mass;
    this->texture = texture;
}

bool MapCell::IsClicked(float x, float y, float size) const {
    return (x >= 0 && x < size && y >= 0 && y < size);
}

void MapCell::SetResource(std::shared_ptr<Resource> res) {
    resource = res;
}

std::shared_ptr<Resource> MapCell::GetResource() const {
    return resource;
}

bool MapCell::HasResource() const {
    return resource != nullptr;
}