#include "Entity.h"

Entity::~Entity()
{
    if (body)
        body->GetWorld()->DestroyBody(body);
}

void Entity::Draw() {
    if (body) {
        // Sync player position with the Box2D body position
        position = { body->GetPosition().x * 50.0f, (1.0f - body->GetPosition().y )* 50.0f}; // Map and invert y for Raylib

        // Print out the current position for debugging purposes
        printf("Player position: (%f, %f)\n", position.x, position.y);

        // Draw the sprite at the updated position
        DrawTextureRec(texture, sourceRect, position, tint);
    }
}

Rectangle Entity::GetCollisionBox() const {
    // Return the collision box based on body dimensions (or sprite size)
    return { position.x, position.y, sourceRect.width * scale, sourceRect.height * scale };
}

Vector2 Entity::NormalizeVector(Vector2 v) {
    float length = sqrt(v.x * v.x + v.y * v.y);
    if (length == 0) return { 0, 0 };
    return { v.x / length, v.y / length };
}


void Player::Update(const float& dt, const std::vector<std::shared_ptr<Entity>>& others, const std::vector<Rectangle>& colliders) {
    Vector2 direction = GetInputDirection();
    b2Vec2 velocity = { direction.x * 5.0f, direction.y * 5.0f };  // Example movement speed
    body->SetLinearVelocity(velocity);
}

Vector2 Player::GetInputDirection() {
    Vector2 direction = { 0, 0 };
    if (IsKeyDown(KEY_W)) direction.y -= 1;
    if (IsKeyDown(KEY_S)) direction.y += 1;
    if (IsKeyDown(KEY_A)) direction.x -= 1;
    if (IsKeyDown(KEY_D)) direction.x += 1;
    return NormalizeVector(direction);  // Normalize to avoid faster diagonal movement
}
void Entity::MoveSafely(Vector2& position, Vector2 velocity, const std::vector<Rectangle>& colliders, float width, float height) {
    float moveDistance = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);
    int steps = std::max(1, (int)ceilf(moveDistance));

    if (steps == 0) return;

    Vector2 step = { velocity.x / steps, velocity.y / steps };

    // Horizontal movement
    for (int i = 0; i < steps; i++) {
        Vector2 trialX = { position.x + step.x, position.y };
        Rectangle boxX = { trialX.x, trialX.y, width, height };
        bool collidedX = false;
        for (const Rectangle& tile : colliders) {
            if (CheckCollisionRecs(boxX, tile)) {
                collidedX = true;
                break;
            }
        }
        if (!collidedX) position.x += step.x;
    }

    // Vertical movement
    for (int i = 0; i < steps; i++) {
        Vector2 trialY = { position.x, position.y + step.y };
        Rectangle boxY = { trialY.x, trialY.y, width, height };
        bool collidedY = false;
        for (const Rectangle& tile : colliders) {
            if (CheckCollisionRecs(boxY, tile)) {
                collidedY = true;
                break;
            }
        }
        if (!collidedY) position.y += step.y;
    }
}
