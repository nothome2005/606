#include "resource.h"

Resource::Resource(int x, int y, ResourceType resourceType_, int mass, Texture* texture)
    : Entity(x, y), resourceType(resourceType_), mass(mass), texture(texture) {
    Entity::type = EntityType::Resource;  // Using explicit base class reference
}

void Resource::Update(float dt) {
    // Ресурсы статичны, не требуется обновление
}

void Resource::Draw(float cellSize, float marginLeft, float marginTop) const {
    if (texture == nullptr || resourceType == ResourceType::None) return;
    
    float cx = marginLeft + x * cellSize;
    float cy = marginTop + y * cellSize;
    
    // Определяем масштаб для отрисовки текстуры
    float scaleX = cellSize / (float)texture->width;
    float scaleY = cellSize / (float)texture->height;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    
    float texW = texture->width * scale;
    float texH = texture->height * scale;
    float offsetX = cx + (cellSize - texW) / 2.0f;
    float offsetY = cy + (cellSize - texH);
    
    DrawTextureEx(*texture, {offsetX, offsetY}, 0, scale, WHITE);
    
    // Если нужно отобразить массу ресурса
    if (mass > 0) {
        DrawText(TextFormat("%d", mass), cx + 5, cy + 5, 20, WHITE);
    }
}

ResourceType Resource::GetResourceType() const {
    return resourceType;
}

int Resource::GetMass() const {
    return mass;
}

void Resource::SetMass(int mass) {
    this->mass = mass;
}

void Resource::SetTexture(Texture* texture) {
    this->texture = texture;
}