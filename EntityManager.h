#pragma once
#include <unordered_map>
#include <string>
#include <set>
#include "Entity.h"
#include "Components.h"
#include "TaskManager/TaskManager.h"
#include <unordered_set>
class EntityManager {
private:
    std::unordered_map<int, std::shared_ptr<Entity>> entities;

public:
    void ResolveCollisions();
    void AddEntity(std::shared_ptr<Entity> entity);
    std::shared_ptr<Entity> GetEntity(const int& id);
    void RemoveEntity(const int& id);
    //void ResolveDynamicCollision(CircleCollider2D* a, CircleCollider2D* b);
    void ResolveDynamicCollision(BoxCollider2D* a, BoxCollider2D* b);
   // void ResolveDynamicCollision(CircleCollider2D* circle, BoxCollider2D* box);
    void HandleCollision(Component* a, Component* b);
  //  void ResolveCollisionsParallel();
    void Update(float dt);
    void Draw();
    void Clear();
};