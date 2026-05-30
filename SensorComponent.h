#pragma once
#include "raylib.h"
#include "TransformComponent.h"
#include <functional>

enum class SensorSide {
    Top,
    Bottom,
    Left,
    Right,
    Custom
};

class SensorComponent : public Collider2D {
public:
    bool debugDraw = false;
    SensorSide side;
    Vector2 customOffset{ 0,0 };  // used if side == Custom
    TransformComponent* transform = nullptr;

    SensorComponent(Entity* owner, float width, float height, SensorSide s)
        : Collider2D(owner), side(s)
    {
        transform = owner->GetComponent<TransformComponent>();

        rect.width = width;
        rect.height = height;
    }

    void SetSize(float w, float h) {
        rect.width = w;
        rect.height = h;
    }

    void SetSide(SensorSide s, Vector2 offset = { 0,0 }) {
        side = s;
        customOffset = offset;
    }

    void Update(float dt) override {
        auto sprite = owner->GetComponent<SpriteComponent>();
        if (!transform) return;

        float x = transform->position.x;
        float y = transform->position.y;

        // Assuming position is top-left of sprite
        if (sprite) {
            switch (side) {
            case SensorSide::Bottom:
                rect.x = x + sprite->sourceRect.width / 2 - rect.width / 2;
                rect.y = y + sprite->sourceRect.height; // bottom of sprite
                break;
            case SensorSide::Top:
                rect.x = x + sprite->sourceRect.width / 2 - rect.width / 2;
                rect.y = y - rect.height;
                break;
            case SensorSide::Left:
                rect.x = x - rect.width;
                rect.y = y + sprite->sourceRect.height / 2 - rect.height / 2;
                break;
            case SensorSide::Right:
                rect.x = x + sprite->sourceRect.width;
                rect.y = y + sprite->sourceRect.height / 2 - rect.height / 2;
                break;
            }
        }
    }


    void ResolveCollision(Collider2D* other) override {
       // if (onCollision) onCollision(other);
    }

    void Draw() override {
        if (debugDraw) {
            Color color = isColliding ? RED : WHITE;

            Rectangle drawRect = rect;


            DrawRectangleLinesEx(drawRect, 1, color);
        }
    }


};