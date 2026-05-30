#pragma once
#include "box2d/box2d.h"
#include "raylib.h"
class Box
{
public:
	Box() {};
	~Box() = default;
	void Init(b2World* world, const Vector2& position, const Vector2& dimensions) {
		//make the body
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(position.x, position.y);
		body = world->CreateBody(&bodyDef);
	
		b2PolygonShape boxShape;
		boxShape.SetAsBox(dimensions.x / 2.0f, dimensions.y / 2.0f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &boxShape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;

		fixture = body->CreateFixture(&fixtureDef);
	}
	b2Body* GetBody() {
		return body;
	}
	b2Fixture* GetFixture() {
		return fixture;
	}
private:
	b2Body* body = nullptr;
	b2Fixture* fixture = nullptr;
};

