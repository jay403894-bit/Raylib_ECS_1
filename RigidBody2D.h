#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include "raylib.h"
#include "PhysicsMaterial.h"
class RigidBody2D : public Component {
public:
    Vector2 acceleration;
    float mass;
    bool useGravity;
    float gravityStrength;

    PhysicsMaterial2D material;

    RigidBody2D(Entity* owner, float mass = 1.0f, bool useGravity = true, float gravityStrength = 980.0f)
        : Component(owner), acceleration({ 0, 0 }), mass(mass), useGravity(useGravity), gravityStrength(gravityStrength) {}

    void ApplyForces(float dt) {
        auto transform = owner->GetComponent<TransformComponent>();
        if (!transform) return;

        // Gravity
        if (useGravity) {
            acceleration.y = gravityStrength / mass;
        }
        else {
            acceleration.y = 0;
        }

        // Integrate acceleration into velocity
        transform->velocity.x += acceleration.x * dt;
        transform->velocity.y += acceleration.y * dt;

        // Clamp fall speed
        if (transform->velocity.y > transform->MAX_VELOCITY)
            transform->velocity.y = transform->MAX_VELOCITY;

        acceleration = { 0, 0 };
    }

    void IntegrateVelocity(float dt) {
        auto transform = owner->GetComponent<TransformComponent>();
        if (!transform) return;

        transform->position.x += transform->velocity.x * dt;
        transform->position.y += transform->velocity.y * dt;
    }

    void Update(float dt) override {


    }
    void Draw() override {}
};