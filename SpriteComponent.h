#pragma once
#include "Component.h"
#include "raylib.h"
#include "AnimationComponent.h"
#include "TransformComponent.h"
#include "TextureManager.h"

class SpriteComponent : public Component {
public:
    Texture2D texture;
    Rectangle sourceRect;   // base frame rectangle (frame 0)
    bool facingRight = true;
    int textureID;  // The texture ID linked to this sprite

    SpriteComponent(Entity* owner, const char* texturePath, const Rectangle& srcRect)
        : Component(owner), sourceRect(srcRect) {
        // Load the texture using the Entity's ID (or you can just use texturePath as the ID here)
        textureID = this->owner->GetID();  // The texture ID is now tied to the entity's unique ID
        TextureManager::LoadTextureID(textureID, texturePath);  // Load the texture via TextureManager

        // Retrieve the texture from the manager
        texture = TextureManager::Get(textureID);
    }
    SpriteComponent(Entity* owner, Texture2D texture, const Rectangle& srcRect)
        : Component(owner), texture(texture), sourceRect(srcRect) {
        // If using TextureManager, you can optionally register this texture with a dummy ID
        textureID = this->owner->GetID();  // The texture ID is now tied to the entity's unique ID
    }
    void Draw() override {
        auto transform = owner->GetComponent<TransformComponent>();
        if (!transform) return;

        // Get the source rectangle for the sprite (from the sprite sheet)
        Rectangle src = sourceRect;

        // Handle animation (if any)
        AnimationComponent* anim = owner->GetComponent<AnimationComponent>();
        if (anim) {
            src.x = anim->GetCurrentFrame() * sourceRect.width;
            src.y = anim->GetCurrentRow() * sourceRect.height;
        }

        // Flip the sprite if it's facing the wrong direction
        if (!facingRight) {
            src.width = -sourceRect.width;  // Flip the width
            src.x += sourceRect.width;      // Offset for proper alignment
        }

        // Apply scale to the destination width and height
        float destWidth = sourceRect.width * transform->scale.x;
        float destHeight = sourceRect.height * transform->scale.y;
        DrawTexturePro(
            texture,
            src,
            {
                transform->position.x,
                transform->position.y,
                destWidth,
                destHeight
            },
    {
        0, 0
    },
            transform->rotation,
            WHITE
        );
    }

    void Update(float dt) override {}

    ~SpriteComponent() {
        UnloadTexture(texture);
    }
};
