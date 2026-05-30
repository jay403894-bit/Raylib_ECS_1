#pragma once
#include "Component.h"
#include <functional>

class Collider2D : public Component {
public:
    bool isTrigger = false;
    bool isStatic = false;
    bool isColliding = false;
    bool wasCollidingLastFrame = false;
    bool debugDraw = false;
    Rectangle rect;

    std::function<void(Collider2D*)> onCollision;

    Collider2D(Entity* owner) : Component(owner) {};
    Collider2D(Entity* owner, Rectangle bounds) : Component(owner) { rect = bounds; };
    virtual void ResolveCollision(Collider2D* other) = 0;
    virtual void Draw() override = 0;
    virtual void Update(float dt) override { isColliding = false; }
};