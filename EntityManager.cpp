#include "EntityManager.h"
#include <thread>
#include <atomic>
#include <unordered_set>

// helpers for unordered_set of pointer-pairs
struct PairHash {
    size_t operator()(const std::pair<void*, void*>& p) const noexcept {
        // combine pointers into size_t hash
        auto h1 = std::hash<uintptr_t>()(reinterpret_cast<uintptr_t>(p.first));
        auto h2 = std::hash<uintptr_t>()(reinterpret_cast<uintptr_t>(p.second));
        // bit-mix
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
    }
};
struct PairEq {
    bool operator()(const std::pair<void*, void*>& a, const std::pair<void*, void*>& b) const noexcept {
        return a.first == b.first && a.second == b.second;
    }
};
int Entity::nextID = 0;

void EntityManager::AddEntity(std::shared_ptr<Entity> entity) {
    entities[entity->id] = entity;
}

std::shared_ptr<Entity> EntityManager::GetEntity(const int& id) {
    auto it = entities.find(id);
    if (it != entities.end()) return it->second;
    return nullptr;
}
void EntityManager::RemoveEntity(const int& id) {
	entities.erase(id);
}
void EntityManager::ResolveDynamicCollision(BoxCollider2D* a, BoxCollider2D* b) {
    auto rbA = a->owner->GetComponent<TransformComponent>();
    auto rbB = b->owner->GetComponent<TransformComponent>();

    if (!rbA || !rbB) return;

    // Step 1: Calculate overlap
    Rectangle rA = a->rect;
    Rectangle rB = b->rect;

    float overlapX = std::min(rA.x + rA.width, rB.x + rB.width) - std::max(rA.x, rB.x);
    float overlapY = std::min(rA.y + rA.height, rB.y + rB.height) - std::max(rA.y, rB.y);

    // Step 2: Separate along the shallowest axis
    if (overlapX < overlapY) {
        float move = overlapX / 2.0f;
        if (rA.x < rB.x) {
            rbA->position.x -= move;
            rbB->position.x += move;
        }
        else {
            rbA->position.x += move;
            rbB->position.x -= move;
        }
        std::swap(rbA->velocity.x, rbB->velocity.x); // optional bounce
    }
    else {
        float move = overlapY / 2.0f;
        if (rA.y < rB.y) {
            rbA->position.y -= move;
            rbB->position.y += move;
        }
        else {
            rbA->position.y += move;
            rbB->position.y -= move;
        }
        std::swap(rbA->velocity.y, rbB->velocity.y); // optional bounce
    }
}

void EntityManager::ResolveCollisions() {
    std::vector<BoxCollider2D*> boxColliders;
    std::vector<SensorComponent*> sensors;
    std::set<std::pair<void*, void*>> currentCollisions;

    auto normalizePair = [](void* a, void* b) -> std::pair<void*, void*> {
        return (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
    };

    // --- Gather all colliders ---
    for (auto& [id, entity] : entities) {
        if (auto* box = entity->GetComponent<BoxCollider2D>()) boxColliders.push_back(box);
        auto entitySensors = entity->GetComponents<SensorComponent>();
        if (!entitySensors.empty()) sensors.insert(sensors.end(), entitySensors.begin(), entitySensors.end());
    }

    // --- Build combined array for sort-and-sweep ---
    struct ColliderAABB { Collider2D* col; Rectangle rect; };
    std::vector<ColliderAABB> allColliders;
    for (auto* box : boxColliders) allColliders.push_back({ box, box->rect });
    for (auto* sensor : sensors) allColliders.push_back({ sensor, sensor->rect });

    // --- Sort by left X ---
    std::sort(allColliders.begin(), allColliders.end(), [](const ColliderAABB& a, const ColliderAABB& b) {
        return a.rect.x < b.rect.x;
    });

    // --- Sort-and-sweep collision check ---
    for (size_t i = 0; i < allColliders.size(); ++i) {
        auto* a = allColliders[i].col;
        float aMaxX = allColliders[i].rect.x + allColliders[i].rect.width;

        for (size_t j = i + 1; j < allColliders.size(); ++j) {
            auto* b = allColliders[j].col;
            float bMinX = allColliders[j].rect.x;

            // No X overlap? Stop checking further (because array is sorted)
            if (bMinX > aMaxX) break;

            // Skip self-collision
            if (a->owner == b->owner) continue;

            // Check full rectangle collision
            if (!CheckCollisionRecs(a->rect, b->rect)) continue;

            auto pair = normalizePair(a, b);
            currentCollisions.insert(pair);

            // --- Resolve collision ---
            BoxCollider2D* boxA = dynamic_cast<BoxCollider2D*>(a);
            BoxCollider2D* boxB = dynamic_cast<BoxCollider2D*>(b);
            SensorComponent* sensorA = dynamic_cast<SensorComponent*>(a);
            SensorComponent* sensorB = dynamic_cast<SensorComponent*>(b);

            // Box vs Box
            if (boxA && boxB) {
                if (!boxA->isStatic && boxB->isStatic) boxA->ResolveCollision(boxB);
                else if (!boxB->isStatic && boxA->isStatic) boxB->ResolveCollision(boxA);
                else {
                    if (boxA->onCollision) boxA->onCollision(boxB);
                    if (boxB->onCollision) boxB->onCollision(boxA);
                }
            }
            // Sensor vs Box
            else if (sensorA && boxB) {
                if (sensorA->onCollision) sensorA->onCollision(boxB);
            }
            else if (sensorB && boxA) {
                if (sensorB->onCollision) sensorB->onCollision(boxA);
            }
            // Sensor vs Sensor (optional)
            else if (sensorA && sensorB) {
                if (sensorA->onCollision) sensorA->onCollision(sensorB);
                if (sensorB->onCollision) sensorB->onCollision(sensorA);
            }
        }
    }

    // --- Update collision states ---
    auto updateState = [&](Collider2D* collider) {
        bool collidingThisFrame = std::any_of(
            currentCollisions.begin(), currentCollisions.end(),
            [collider](const auto& pair) { return pair.first == collider || pair.second == collider; }
        );

        bool justStarted = !collider->wasCollidingLastFrame && collidingThisFrame;
        bool justEnded = collider->wasCollidingLastFrame && !collidingThisFrame;

        if (justStarted) std::cout << "STARTED collision: " << collider->owner->id << "\n";
        else if (justEnded) std::cout << "ENDED collision: " << collider->owner->id << "\n";

        collider->wasCollidingLastFrame = collidingThisFrame;
        collider->isColliding = collidingThisFrame;
    };

    for (auto* box : boxColliders) updateState(box);
    for (auto* sensor : sensors) updateState(sensor);
}


/*
void EntityManager::ResolveCollisions() {
    std::vector<BoxCollider2D*> boxColliders;
    std::vector<SensorComponent*> sensors;

    // gather colliders (copies of pointers only)
    for (auto& [id, entity] : entities) {
        if (auto* box = entity->GetComponent<BoxCollider2D>()) boxColliders.push_back(box);
        auto entitySensors = entity->GetComponents<SensorComponent>();
        if (!entitySensors.empty()) sensors.insert(sensors.end(), entitySensors.begin(), entitySensors.end());
    }

    // build combined readonly array for broad-phase
    struct ColliderAABB { Collider2D* col; Rectangle rect; };
    std::vector<ColliderAABB> allColliders;
    allColliders.reserve(boxColliders.size() + sensors.size());
    for (auto* box : boxColliders) {
        if (box) allColliders.push_back({ box, box->rect });
    }
    for (auto* s : sensors) {
        if (s) allColliders.push_back({ s, s->rect });
    }

    if (allColliders.empty()) {
        // nothing to do
        return;
    }

    // sort by left X (read-only sort of local vector)
    std::sort(allColliders.begin(), allColliders.end(), [](const ColliderAABB& a, const ColliderAABB& b) {
        return a.rect.x < b.rect.x;
    });

    // choose thread count (cap at number of colliders to avoid tiny chunks)
    unsigned int hw = std::thread::hardware_concurrency();
    if (hw == 0) hw = 2;
    size_t numThreads = std::min<size_t>(hw, std::max<size_t>(1, allColliders.size()));
    size_t chunk = (allColliders.size() + numThreads - 1) / numThreads;

    // per-thread candidate pair vectors
    std::vector<std::vector<std::pair<void*, void*>>> threadPairs(numThreads);

    // launch threads for detection (read-only)
    std::vector<std::shared_ptr<BaseTask>> tasks;
    tasks.reserve(numThreads);

    for (size_t t = 0; t < numThreads; ++t) {
        size_t start = t * chunk;
        size_t end = std::min(start + chunk, allColliders.size());
        if (start >= end) {
            // nothing for this thread
            continue;
        }

        auto collider_task = std::make_shared<Task>(
            [start,
            end,
            allCollidersPtr = &allColliders,
            threadPairsPtr = &threadPairs,
            t]()
            {
                auto& allColliders = *allCollidersPtr;
                auto& threadPairs = *threadPairsPtr;
                auto& out = threadPairs[t];
                out.reserve((end - start) * 2);

                for (size_t i = start; i < end; ++i) {
                    Collider2D* a = allColliders[i].col;
                    if (!a) continue;
                    float aMaxX = allColliders[i].rect.x + allColliders[i].rect.width;

                    for (size_t j = i + 1; j < allColliders.size(); ++j) {
                        if (allColliders[j].rect.x > aMaxX) break;
                        Collider2D* b = allColliders[j].col;
                        if (!b) continue;
                        if (a->owner == b->owner) continue;
                        if (!CheckCollisionRecs(a->rect, b->rect)) continue;

                        void* p1 = (a < b ? a : b);
                        void* p2 = (a < b ? b : a);
                        out.emplace_back(p1, p2);
                    }
                }
            });

        tasks.push_back(collider_task);
        TaskManager::Get()->AddTask(collider_task);
    }

    // join threads
    //for (auto& th : workers) {
   //     if (th.joinable()) th.join();
  //  }
 //   for (auto& task : tasks) {
     //   while (!task->IsCompleted())
    //    {

   //     }
  //  }
    // merge thread results into a single unique set
    std::unordered_set<std::pair<void*, void*>, PairHash, PairEq> uniquePairs;
    for (auto& vec : threadPairs) {
        for (auto& pr : vec) uniquePairs.insert(pr);
    }

    // --- Now single-threaded: process pairs (narrow phase + resolution) ---
    // Convert set to vector for iteration
    std::vector<std::pair<void*, void*>> pairs;
    pairs.reserve(uniquePairs.size());
    for (auto& pr : uniquePairs) pairs.push_back(pr);

    // We'll also build a set of colliders that are colliding this frame for the updateState step
    std::unordered_set<Collider2D*> collidingColliders;
    collidingColliders.reserve(pairs.size() * 2);

    for (auto& pr : pairs) {
        Collider2D* a = reinterpret_cast<Collider2D*>(pr.first);
        Collider2D* b = reinterpret_cast<Collider2D*>(pr.second);
        if (!a || !b) continue;

        // mark colliding
        collidingColliders.insert(a);
        collidingColliders.insert(b);

        // classify types
        BoxCollider2D* boxA = dynamic_cast<BoxCollider2D*>(a);
        BoxCollider2D* boxB = dynamic_cast<BoxCollider2D*>(b);
        SensorComponent* sensorA = dynamic_cast<SensorComponent*>(a);
        SensorComponent* sensorB = dynamic_cast<SensorComponent*>(b);

        // Box vs Box
        if (boxA && boxB) {
            // if both static we skip; mirrors your logic
            if (!boxA->isStatic && boxB->isStatic) boxA->ResolveCollision(boxB);
            else if (!boxB->isStatic && boxA->isStatic) boxB->ResolveCollision(boxA);
            else {
                if (boxA->onCollision) boxA->onCollision(boxB);
                if (boxB->onCollision) boxB->onCollision(boxA);
            }
            continue;
        }

        // Sensor vs Box
        if (sensorA && boxB) {
            if (sensorA->onCollision) sensorA->onCollision(boxB);
            // don't call boxB->onCollision(sensorA) unless you specifically want it
            continue;
        }
        if (sensorB && boxA) {
            if (sensorB->onCollision) sensorB->onCollision(boxA);
            continue;
        }

        // Sensor vs Sensor
        if (sensorA && sensorB) {
            if (sensorA->onCollision) sensorA->onCollision(sensorB);
            if (sensorB->onCollision) sensorB->onCollision(sensorA);
            continue;
        }
    }

    // --- Update collider states (wasColliding/isColliding) for boxes + sensors ---
    auto updateState = [&](Collider2D* collider) {
        if (!collider) return;
        bool collidingThisFrame = collidingColliders.find(collider) != collidingColliders.end();
        bool justStarted = !collider->wasCollidingLastFrame && collidingThisFrame;
        bool justEnded = collider->wasCollidingLastFrame && !collidingThisFrame;

        if (justStarted) std::cout << "STARTED collision: " << collider->owner->id << "\n";
        else if (justEnded) std::cout << "ENDED collision: " << collider->owner->id << "\n";

        collider->wasCollidingLastFrame = collidingThisFrame;
        collider->isColliding = collidingThisFrame;
    };

    for (auto* box : boxColliders) updateState(box);
    for (auto* s : sensors) updateState(s);
}

void EntityManager::ResolveCollisions() {
    std::vector<BoxCollider2D*> boxColliders;
    std::vector<SensorComponent*> sensors;

    // gather colliders (copies of pointers only)
    for (auto& [id, entity] : entities) {
        if (auto* box = entity->GetComponent<BoxCollider2D>()) boxColliders.push_back(box);
        auto entitySensors = entity->GetComponents<SensorComponent>();
        if (!entitySensors.empty()) sensors.insert(sensors.end(), entitySensors.begin(), entitySensors.end());
    }

    // build combined readonly array for broad-phase
    struct ColliderAABB { Collider2D* col; Rectangle rect; };
    std::vector<ColliderAABB> allColliders;
    allColliders.reserve(boxColliders.size() + sensors.size());
    for (auto* box : boxColliders) {
        if (box) allColliders.push_back({ box, box->rect });
    }
    for (auto* s : sensors) {
        if (s) allColliders.push_back({ s, s->rect });
    }

    if (allColliders.empty()) {
        // nothing to do
        return;
    }

    // sort by left X (read-only sort of local vector)
    std::sort(allColliders.begin(), allColliders.end(), [](const ColliderAABB& a, const ColliderAABB& b) {
        return a.rect.x < b.rect.x;
        });

    // choose thread count (cap at number of colliders to avoid tiny chunks)
    unsigned int hw = std::thread::hardware_concurrency();
    if (hw == 0) hw = 2;
    size_t numThreads = std::min<size_t>(hw, std::max<size_t>(1, allColliders.size()));
    size_t chunk = (allColliders.size() + numThreads - 1) / numThreads;

    // per-thread candidate pair vectors
    std::vector<std::vector<std::pair<void*, void*>>> threadPairs(numThreads);

    // launch threads for detection (read-only)
    std::vector<std::thread> workers;
    workers.reserve(numThreads);
    for (size_t t = 0; t < numThreads; ++t) {
        size_t start = t * chunk;
        size_t end = std::min(start + chunk, allColliders.size());
        if (start >= end) {
            // nothing for this thread
            continue;
        }

        workers.emplace_back([start, end, &allColliders, &threadPairs, t]() {
            auto& out = threadPairs[t];
            out.reserve((end - start) * 2);

            for (size_t i = start; i < end; ++i) {
                Collider2D* a = allColliders[i].col;
                if (!a) continue;
                float aMaxX = allColliders[i].rect.x + allColliders[i].rect.width;

                // j begins at next element after i (global)
                for (size_t j = i + 1; j < allColliders.size(); ++j) {
                    // early out on x (sorted by left X)
                    if (allColliders[j].rect.x > aMaxX) break;

                    Collider2D* b = allColliders[j].col;
                    if (!b) continue;

                    // skip same-owner shortcuts here (cheap)
                    if (a->owner == b->owner) continue;

                    // full rect test (safe read-only)
                    if (!CheckCollisionRecs(a->rect, b->rect)) continue;

                    // normalize order so pair is stable (pointer order)
                    void* p1 = a < b ? a : b;
                    void* p2 = a < b ? b : a;
                    out.emplace_back(p1, p2);
                }
            }
            });
    }

    // join threads
    for (auto& th : workers) {
        if (th.joinable()) th.join();
    }

    // merge thread results into a single unique set
    std::unordered_set<std::pair<void*, void*>, PairHash, PairEq> uniquePairs;
    for (auto& vec : threadPairs) {
        for (auto& pr : vec) uniquePairs.insert(pr);
    }

    // --- Now single-threaded: process pairs (narrow phase + resolution) ---
    // Convert set to vector for iteration
    std::vector<std::pair<void*, void*>> pairs;
    pairs.reserve(uniquePairs.size());
    for (auto& pr : uniquePairs) pairs.push_back(pr);

    // We'll also build a set of colliders that are colliding this frame for the updateState step
    std::unordered_set<Collider2D*> collidingColliders;
    collidingColliders.reserve(pairs.size() * 2);

    for (auto& pr : pairs) {
        Collider2D* a = reinterpret_cast<Collider2D*>(pr.first);
        Collider2D* b = reinterpret_cast<Collider2D*>(pr.second);
        if (!a || !b) continue;

        // mark colliding
        collidingColliders.insert(a);
        collidingColliders.insert(b);

        // classify types
        BoxCollider2D* boxA = dynamic_cast<BoxCollider2D*>(a);
        BoxCollider2D* boxB = dynamic_cast<BoxCollider2D*>(b);
        SensorComponent* sensorA = dynamic_cast<SensorComponent*>(a);
        SensorComponent* sensorB = dynamic_cast<SensorComponent*>(b);

        // Box vs Box
        if (boxA && boxB) {
            // if both static we skip; mirrors your logic
            if (!boxA->isStatic && boxB->isStatic) boxA->ResolveCollision(boxB);
            else if (!boxB->isStatic && boxA->isStatic) boxB->ResolveCollision(boxA);
            else {
                if (boxA->onCollision) boxA->onCollision(boxB);
                if (boxB->onCollision) boxB->onCollision(boxA);
            }
            continue;
        }

        // Sensor vs Box
        if (sensorA && boxB) {
            if (sensorA->onCollision) sensorA->onCollision(boxB);
            // don't call boxB->onCollision(sensorA) unless you specifically want it
            continue;
        }
        if (sensorB && boxA) {
            if (sensorB->onCollision) sensorB->onCollision(boxA);
            continue;
        }

        // Sensor vs Sensor
        if (sensorA && sensorB) {
            if (sensorA->onCollision) sensorA->onCollision(sensorB);
            if (sensorB->onCollision) sensorB->onCollision(sensorA);
            continue;
        }
    }

    // --- Update collider states (wasColliding/isColliding) for boxes + sensors ---
    auto updateState = [&](Collider2D* collider) {
        if (!collider) return;
        bool collidingThisFrame = collidingColliders.find(collider) != collidingColliders.end();
        bool justStarted = !collider->wasCollidingLastFrame && collidingThisFrame;
        bool justEnded = collider->wasCollidingLastFrame && !collidingThisFrame;

        if (justStarted) std::cout << "STARTED collision: " << collider->owner->id << "\n";
        else if (justEnded) std::cout << "ENDED collision: " << collider->owner->id << "\n";

        collider->wasCollidingLastFrame = collidingThisFrame;
        collider->isColliding = collidingThisFrame;
        };

    for (auto* box : boxColliders) updateState(box);
    for (auto* s : sensors) updateState(s);
}
*/
void EntityManager::Update(float dt) {
    // STEP 1: Apply forces
    for (auto& [id, entity] : entities) {
        auto rb = entity->GetComponent<RigidBody2D>();
        if (rb) rb->ApplyForces(dt);
    }

    // STEP 2: Move entities
    for (auto& [id, entity] : entities) {
        auto rb = entity->GetComponent<RigidBody2D>();
        if (rb) rb->IntegrateVelocity(dt);
    }

    // STEP 3: Sync collider rects
    for (auto& [id, entity] : entities) {
        auto collider = entity->GetComponent<BoxCollider2D>();
        auto transform = entity->GetComponent<TransformComponent>();
        if (collider && transform) {
           // collider->rect.x = transform->position.x - collider->rect.width / 2.0f;
          // collider->rect.y = transform->position.y - collider->rect.height / 2.0f;
            collider->rect.x = transform->position.x;
            collider->rect.y = transform->position.y;
        }
    }

    // STEP 4: Resolve collisions
    ResolveCollisions();

    // STEP 5: Draw and other updates
    for (auto& [id, entity] : entities) {
        entity->Update(dt);
    }
}

void EntityManager::Draw() {
    // First: draw entities without sprites (e.g., collidaaaaaaaaaaaaaaaers with debugDraw)
    for (auto& [id, entity] : entities) {
        auto sprite = entity->GetComponent<SpriteComponent>();
        if (!sprite) {
            entity->Draw();  // This will call BoxCollider2D::Draw() if debugDraw is true
        }
    }

    // Second: collect drawable entities that *do* have a sprite
    std::vector<Entity*> drawableEntities;

    for (auto& [id, entity] : entities) {
        auto transform = entity->GetComponent<TransformComponent>();
        auto sprite = entity->GetComponent<SpriteComponent>();
        if (transform && sprite) {
            drawableEntities.push_back(entity.get());
        }
    }

    // Sort by layer
    std::sort(drawableEntities.begin(), drawableEntities.end(), [](Entity* a, Entity* b) {
        return a->GetComponent<TransformComponent>()->layer < b->GetComponent<TransformComponent>()->layer;
        });

    // Draw sorted sprite entities
    for (Entity* entity : drawableEntities) {
        entity->Draw();
    }
}
void EntityManager::Clear() {
    entities.clear();
}