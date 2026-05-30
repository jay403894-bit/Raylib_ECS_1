#pragma once
#include "TransformComponent.h"
#include "RigidBody2D.h"
#include "raylib.h"
#include <functional>
#include <iostream>

class BoxCollider2D : public Collider2D {
public:
    BoxCollider2D(Entity* owner, Rectangle bounds)
        : Collider2D(owner, bounds) {

    }

    void Update(float dt) override {
        if (TransformComponent* transform = owner->GetComponent<TransformComponent>()) {
            // Position collider rect so that its bottom-center aligns with transform->position
            rect.x = transform->position.x;
            rect.y = transform->position.y;  // bottom aligns with position.y
        }
    }


    void Draw() override {
        if (debugDraw) {
            Color color = isColliding ? RED : WHITE;         

            DrawRectangleLinesEx(rect, 1, color);
        }
    }
    void ResolveCollision(Collider2D* other) override {
        if (auto* otherBox = dynamic_cast<BoxCollider2D*>(other)) {
            // === Box vs Box ===
            if (!CheckCollisionRecs(rect, otherBox->rect)) return;

            auto* transform = owner->GetComponent<TransformComponent>();
            if (!transform || isStatic) return;

            auto* otherTransform = other->owner->GetComponent<TransformComponent>();
            if (!otherTransform) return;

            auto* rb = other->owner->GetComponent<RigidBody2D>();
            auto* tr = transform; // reuse transform pointer
            float restitution = rb ? rb->material.restitution : 0.0f;
            float friction = rb ? rb->material.friction : 0.0f;

            float skin = 1.5f; // pixels of tolerance
            float overlapX = std::min(rect.x + rect.width, otherBox->rect.x + otherBox->rect.width)
                - std::max(rect.x, otherBox->rect.x);
            float overlapY = std::min(rect.y + rect.height, otherBox->rect.y + otherBox->rect.height)
                - std::max(rect.y, otherBox->rect.y);

            if (overlapX <= skin && overlapY <= skin) return;
            if (overlapX <= 0 || overlapY <= 0) return;

            bool resolvedX = overlapX < overlapY;

            if (resolvedX) {
                if (rect.x + rect.width / 2.0f < otherBox->rect.x + otherBox->rect.width / 2.0f)
                    transform->position.x -= overlapX;
                else
                    transform->position.x += overlapX;

                if (rb) transform->velocity.x *= -restitution;
                else transform->velocity.x = 0;
            }
            else {
                if (rect.y + rect.height / 2.0f < otherBox->rect.y + otherBox->rect.height / 2.0f)
                    transform->position.y -= overlapY;
                else
                    transform->position.y += overlapY;

                if (rb) {
                    transform->velocity.y *= -restitution;
                    if (friction > 0) {
                        float sign = (tr->velocity.x > 0) ? 1.0f : -1.0f;
                        float reduction = friction * 10.0f * 0.016f; // first number was 10
                        if (std::abs(tr->velocity.x) < reduction) transform->velocity.x = 0;
                        else transform->velocity.x -= sign * reduction;
                    }
                }
                else {
                    tr->velocity.y = 0;
                }
            }

            // Sync collider
            rect.x = transform->position.x;
            rect.y = transform->position.y;
        }
    }
};
    /*
    void ResolveCollision(Collider2D* other) {
        auto* otherBox = dynamic_cast<BoxCollider2D*>(other);
        if (!otherBox) return;

        if (!CheckCollisionRecs(rect, otherBox->rect)) return;

        if (other->isTrigger) {
            if (onCollision) onCollision(otherBox);
            return;
        }

        auto transform = owner->GetComponent<TransformComponent>();
        if (!transform || isStatic) return;

        auto otherTransform = other->owner->GetComponent<TransformComponent>();
        if (!otherTransform) return;

        // Calculate overlap on both axes
        float overlapX = std::min(rect.x + rect.width, otherBox->rect.x + otherBox->rect.width)
            - std::max(rect.x, otherBox->rect.x);
        float overlapY = std::min(rect.y + rect.height, otherBox->rect.y + otherBox->rect.height)
            - std::max(rect.y, otherBox->rect.y);

        // Determine direction of resolution
        float centerAX = rect.x + rect.width / 2.0f;
        float centerBX = otherBox->rect.x + otherBox->rect.width / 2.0f;
        float mtvX = (centerAX < centerBX) ? -overlapX : overlapX;

        float centerAY = rect.y + rect.height / 2.0f;
        float centerBY = otherBox->rect.y + otherBox->rect.height / 2.0f;
        float mtvY = (centerAY < centerBY) ? -overlapY : overlapY;

        // Resolve using the smaller overlap
        bool resolvedX = std::abs(overlapX) < std::abs(overlapY);
        if (resolvedX) {
            if (mtvX > 0)
                transform->position.x = otherBox->rect.x + otherBox->rect.width;
            else
                transform->position.x = otherBox->rect.x - rect.width;
        }
        else {
            if (mtvY > 0)
                transform->position.y = otherBox->rect.y + otherBox->rect.height;
            else
                transform->position.y = otherBox->rect.y - rect.height;
        }
        // Apply restitution and friction
        auto rb = owner->GetComponent<RigidBody2D>();
        if (rb) {
            float restitution = rb->material.restitution;
            float friction = rb->material.friction;

            if (resolvedX) {
                transform->velocity.x *= -restitution;

                // Only apply friction if horizontal collision & grounded
                // (e.g., running into a wall while standing)
                transform->velocity.y *= 1.0f; // No friction in Y here
            }
            else {
                transform->velocity.y *= -restitution;
            }
        }
        else {
            // Default: stop motion
            if (resolvedX)
                transform->velocity.x = 0;
            else
                transform->velocity.y = 0;
        }

        // Sync the collider with the updated transform
        rect.x = transform->position.x;
        rect.y = transform->position.y;

        if (onCollision) onCollision(otherBox);
    }*/
