// InputComponent.h
#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "SensorComponent.h"
#include "raylib.h"

class InputComponent : public Component {
public:
    const int MAX_JUMPS = 2;
    int acceleration = 100;
    int jumps = MAX_JUMPS;
    bool grounded = false;
    bool isWallSliding = false;
    bool touchingLeftWall = false;
    bool touchingRightWall = false;

    InputComponent(Entity* owner) : Component(owner) {}

     void OnSensorCollision(SensorComponent* sensor, Collider2D* other) {
        if (!sensor || !other) return;
        if (other->owner->id == owner->id) return;
        switch (sensor->side) {
        case SensorSide::Bottom:
            grounded = true;
            jumps = MAX_JUMPS;
            break;
        case SensorSide::Left:
            if ( IsKeyDown(KEY_A)) {
                if(!grounded)
                    isWallSliding = true;
                touchingLeftWall = true;
            }
            break;
        case SensorSide::Right:
            if (IsKeyDown(KEY_D)) {
                if(!grounded)
                    isWallSliding = true;
                touchingRightWall = true;
            }
            break;
        default:    break;
        }
    }

    void Update(float dt) override {
        auto transform = owner->GetComponent<TransformComponent>();
        auto sprite = owner->GetComponent<SpriteComponent>();
        if (!transform || !sprite) return;

        // Horizontal movement
        if (IsKeyDown(KEY_A)) {
            if(transform->velocity.x > (-1.0f*transform->MAX_VELOCITY))
                transform->velocity.x -= acceleration * dt;
            sprite->facingRight = false;
        }
        if (IsKeyDown(KEY_D)) {
            if(transform->velocity.x < transform->MAX_VELOCITY)
                transform->velocity.x += acceleration * dt;
            sprite->facingRight = true;
        }

        // Jumping
        if (IsKeyPressed(KEY_SPACE) && jumps > 0) {
           std::cout << "\n Jumps: " << jumps;
            jumps--;
            std::cout << "\n Jumps Remaining: " << jumps;
            grounded = false;
            transform->velocity.y = -250;
        }

        // Wall slide clamp
        if (isWallSliding) {
            const float MAX_SLIDE_SPEED = 100.0f;
            if (transform->velocity.y > MAX_SLIDE_SPEED)
                transform->velocity.y = MAX_SLIDE_SPEED;
            if (IsKeyPressed(KEY_SPACE) && IsKeyDown(KEY_A)) {
                transform->velocity.y = -250;
                transform->velocity.x = 100;
            }
            if (IsKeyPressed(KEY_SPACE) && IsKeyDown(KEY_D)) {
                transform->velocity.y = -250;
                transform->velocity.x = -100;
            }
            // Reset wall slide if:
            if ((touchingLeftWall && IsKeyDown(KEY_D)) || // moving away from left wall
                (touchingRightWall && IsKeyDown(KEY_A)) || // moving away from right wall
                (!touchingLeftWall && !touchingRightWall) || // not touching any wall
                transform->velocity.y <= 0) // moving up or stationary
            {
                isWallSliding = false;
                touchingLeftWall = false;
                touchingRightWall = false;
            }
        }
    }
    void Draw() override {}
};
