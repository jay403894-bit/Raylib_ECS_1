#pragma once
#include "Components.h"
#include "raylib.h"
#include <functional>
#include <iostream>

class BoxCollider2D : public Collider2D {
public:
    Rectangle rect;
    bool isTrigger = false;
    bool isStatic = false;
    bool debugDraw = false;
    bool isColliding = false;
    bool wasCollidingLastFrame = false;

    std::function<void(BoxCollider2D* other)> onCollision = nullptr;

    BoxCollider2D(Entity* owner, Rectangle bounds)
        : COllider(owner), rect(bounds) {
    }

    void Update(float dt) override {
        auto transform = owner->GetComponent<TransformComponent>();
        if (transform) {
            rect.x = transform->position.x - rect.width / 2.0f;
            rect.y = transform->position.y - rect.height / 2.0f;
        }
    }


    void Draw() override {
        if (debugDraw) {
            Color color = isColliding ? RED : WHITE;
            DrawRectangleLinesEx(rect, 1, color);
        }
    }

    void ResolveCollision(BoxCollider2D* other) {
        if (!CheckCollisionRecs(rect, other->rect)) return;

        if (other->isTrigger) {
            if (onCollision) onCollision(other);
            return;
        }

        auto transform = owner->GetComponent<TransformComponent>();
        if (!transform || isStatic) return;

        auto otherTransform = other->owner->GetComponent<TransformComponent>();
        if (!otherTransform) return;

        // Calculate overlap on both axes
        float overlapX = std::min(rect.x + rect.width, other->rect.x + other->rect.width)
            - std::max(rect.x, other->rect.x);
        float overlapY = std::min(rect.y + rect.height, other->rect.y + other->rect.height)
            - std::max(rect.y, other->rect.y);

        // Determine direction of resolution
        float centerAX = rect.x + rect.width / 2.0f;
        float centerBX = other->rect.x + other->rect.width / 2.0f;
        float mtvX = (centerAX < centerBX) ? -overlapX : overlapX;

        float centerAY = rect.y + rect.height / 2.0f;
        float centerBY = other->rect.y + other->rect.height / 2.0f;
        float mtvY = (centerAY < centerBY) ? -overlapY : overlapY;

        // Resolve using the smaller overlap
        if (std::abs(overlapX) < std::abs(overlapY)) {
            // Push out horizontally and zero horizontal velocity
            transform->position.x += mtvX;

            // Snap flush to avoid tiny overlaps
            if (mtvX > 0)
                transform->position.x = other->rect.x + other->rect.width;
            else
                transform->position.x = other->rect.x - rect.width;

            transform->velocity.x = 0;
        }
        else {
            // Push out vertically and zero vertical velocity
            transform->position.y += mtvY;

            if (mtvY > 0)
                transform->position.y = other->rect.y + other->rect.height; // push 1 pixel extra down
            else
                transform->position.y = other->rect.y - rect.height; // push 1 pixel extra up

            transform->velocity.y = 0;
        }

        // Sync the collider rectangle with updated transform
        rect.x = transform->position.x;
        rect.y = transform->position.y;

        if (onCollision) onCollision(other);
    }
};