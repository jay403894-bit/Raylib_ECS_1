#pragma once
#include "Component.h"
#include "Entity.h"
#include <string>
class TagComponent : public Component {
public:
	std::string tag;
	
	TagComponent(Entity* owner, std::string tag) : Component(owner), tag(tag) {};
	virtual ~TagComponent() = default;
	std::string GetTag() { return tag; };
	void SetTag(const std::string& strIn) { tag = strIn; };
	virtual void Update(float dt) {};
	virtual void Draw() {};
	
};