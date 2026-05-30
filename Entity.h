#pragma once
#include "Utilities/Object.h"
#include "Component.h"
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>

class Entity {
public:
    // map from type to a vector of components
    std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> components;
    int id;
    static int nextID;

    Entity() {
        id = nextID++;
    }

    int GetID() { return id; }

    // Add a new component (can add multiple of the same type)
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        auto comp = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* ptr = comp.get();
        components[std::type_index(typeid(T))].push_back(std::move(comp));
        return ptr;
    }

    // Get the first component of this type (for backwards compatibility)
    template<typename T>
    T* GetComponent() {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end() && !it->second.empty()) {
            return static_cast<T*>(it->second[0].get());
        }
        return nullptr;
    }

    // Get all components of this type
    template<typename T>
    std::vector<T*> GetComponents() {
        std::vector<T*> out;
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            for (auto& comp : it->second)
                out.push_back(static_cast<T*>(comp.get()));
        }
        return out;
    }


    void Update(float dt) {
        std::vector<Component*> comps;
        for (auto& [type, vec] : components) {
            for (auto& comp : vec)
                comps.push_back(comp.get());
        }

        std::sort(comps.begin(), comps.end(),
            [](Component* a, Component* b) {
                return a->priority < b->priority;
            });

        for (auto* comp : comps)
            comp->Update(dt);
    }

    void Draw() {
        std::vector<Component*> comps;
        for (auto& [type, vec] : components) {
            for (auto& comp : vec)
                comps.push_back(comp.get());
        }

        std::sort(comps.begin(), comps.end(),
            [](Component* a, Component* b) {
                return a->priority < b->priority;
            });

        for (auto* comp : comps)
            comp->Draw();
    }
    /*/
        void Update(float dt) {
        for (auto& [type, vec] : components) {
            for (auto& comp : vec)
                comp->Update(dt);
        }
    }
    void Draw() {
        for (auto& [type, vec] : components) {
            for (auto& comp : vec)
                comp->Draw();
        }
    }*/
};
/*
#pragma once
#include "Utilities/Object.h"
#include "Component.h"
#include <typeindex>
#include <unordered_map>
#include <memory>

class Entity {
public:
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
    int id;
    static int nextID;

    Entity() {
        id = nextID++;
    }
    int GetID() {
        return id;
    }
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        auto comp = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* ptr = comp.get();
        components[std::type_index(typeid(T))] = std::move(comp);
        return ptr;
    }

    template<typename T>
    T* GetComponent() {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    void Update(float dt) {
        for (auto& [type, comp] : components) {
            comp->Update(dt);
        }
    }

    void Draw() {
        for (auto& [type, comp] : components) {
            comp->Draw();
        }
    }
};

*/