#pragma once

#include "Graphics/AABB.h"

class IPFInteractable
{
public:
    void OnClick() = 0;
    void OnDrag(){};
    void UpdateBounds() = 0;
    AABB GetBounds() { return Bounds; }
    void DrawVisualisation(){};
private:
    AABB Bounds;
};