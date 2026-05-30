#pragma once
#include "raylib.h"
#include "Components.h"

class TransformComponent : public Component {
public:
    Vector2 position;
    const float MAX_VELOCITY = 300;
    Vector2 velocity;
    Vector2 scale;
    float rotation;
    int layer;
    TransformComponent(Entity* owner, Vector2 pos, Vector2 initScale = { 1.0f, 1.0f }, int layer = 0)
        : Component(owner), position(pos), velocity({ 0, 0 }), rotation(0), scale(initScale), layer(layer) {
    }

    void Update(float dt) override {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;

    }

    void Draw() override {
        // No drawing needed here
    }
};