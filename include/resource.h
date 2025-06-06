#pragma once
#include "entity.h"
#include "map_cell.h"

class Resource : public Entity {
public:
    Resource(int x, int y, ResourceType resourceType_, int mass, Texture* texture);
    
    void Update(float dt) override;
    void Draw(float cellSize, float marginLeft, float marginTop) const override;
    
    ResourceType GetResourceType() const;
    int GetMass() const;
    void SetMass(int mass);
    void SetTexture(Texture* texture);

private:
    ResourceType resourceType;
    int mass;
    Texture* texture;
};