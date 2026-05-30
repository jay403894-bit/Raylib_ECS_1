#pragma once
#include "raylib.h"

struct Tile {
	int id;
	Rectangle srcRect;
	bool solid = false;
	PhysicsMaterial2D material;
};