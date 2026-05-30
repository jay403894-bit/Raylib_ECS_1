#pragma once
struct PhysicsMaterial2D {
    float restitution = 0.0f; // 0 = no bounce, 1 = perfect bounce
    float friction = 0.5f; // 0 = slippery, 1 = sticky
    bool solid;        // Solid for collision
    bool destructible; //destructible
};