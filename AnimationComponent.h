#pragma once
#include "Component.h"
#include <map>
#include <string>

class AnimationComponent : public Component {
public:
    struct Animation {
        int startFrame;
        int endFrame;
        float frameTime;
        float elapsedTime = 0.0f;
        int row;

        Animation(int start = 0, int end = 0, float time = 1.0f, int row = 0)
            : startFrame(start), endFrame(end), frameTime(time), row(row) {
        }

        void Update(float dt) {
            elapsedTime += dt;
            int frameCount = endFrame - startFrame + 1;
            if (elapsedTime >= frameTime * frameCount) {
                elapsedTime = 0.0f;
            }
        }

        int GetCurrentFrame() const {
            int totalFrames = endFrame - startFrame + 1;
            return startFrame + static_cast<int>((elapsedTime / frameTime)) % totalFrames;
        }
    };

private:
    std::map<std::string, Animation> animations;
    std::string currentAnimation = "idle";

public:
    int currentFrame = 0;

    AnimationComponent(Entity* owner)
        : Component(owner) {
    }

    void AddAnimation(const std::string& name, int start, int end, float time, int row = 0) {
        animations[name] = Animation(start, end, time, row);
    }

    void SetAnimation(const std::string& name) {
        if (name != currentAnimation && animations.find(name) != animations.end()) {
            currentAnimation = name;
            animations[currentAnimation].elapsedTime = 0.0f;  // Reset timing when switching
        }
    }

    void Update(float dt) override {
        if (animations.find(currentAnimation) == animations.end()) return;

        Animation& anim = animations[currentAnimation];
        anim.Update(dt);
        currentFrame = anim.GetCurrentFrame();
    }

    void Draw() override {
        // This component does not draw directly — SpriteComponent should query frame/row from this
    }

    int GetCurrentFrame() const {
        if (animations.find(currentAnimation) == animations.end()) return 0;
        return animations.at(currentAnimation).GetCurrentFrame();
    }

    int GetCurrentRow() const {
        if (animations.find(currentAnimation) == animations.end()) return 0;
        return animations.at(currentAnimation).row;
    }
};