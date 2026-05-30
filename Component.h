#pragma once
#include <memory>
//forward declaration
class Entity;

class Component {
public:
    Entity* owner;
    int priority = 0;

    Component(Entity* owner) : owner(owner) {}
    virtual ~Component() = default;

    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;
};



