#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "raylib.h"
#include "FontManager.h"
#include "SceneManager.h"
#include "EntityManager.h"
#include "Tilemap.h"
#include "Entity.h"
#include "Components.h"
#include "TaskManager/TaskManager.h"

struct SpawnPoint {
	std::string type;  // e.g., "player", "enemy"
	Vector2 position;
};
const float PIXELS_PER_METER = 50.0f;
const float METER_PER_PIXEL = (1.0f / PIXELS_PER_METER);


class StartMenu : public Scene
{
private:
	int menuIndex = 0;
    bool quit = false;
	std::shared_ptr<Font> fnt;
	Rectangle newGameButton = { 130, 300, 280, 32 };
	Rectangle continueButton = { 130, 342, 280, 32 };
	Rectangle optionsButton = { 130, 384, 280, 32 };
	Rectangle quitButton = { 130, 426, 280, 32 };
	Rectangle titleBG = { 80, 10, 390, 60 };
	Rectangle menuBG = { 100, 260, 340, 240 };
public:
	StartMenu();
	virtual void Update(bool& isRunning, float dt = 0.0f) override;
	virtual void Draw() override;
	virtual void HandleInput() override;
	virtual ~StartMenu() override;
	void NewGamePressed();
	void ContinuePressed();
	void OptionsPressed();
	void QuitPressed();
};

class TestScene : public Scene {
private:
	std::shared_ptr<EntityManager> entities;
	Tilesheet sheet;
	Tilemap tmap;
	int playerID;
	Camera2D camera = { 0 };
	std::shared_ptr<Entity> playerptr;
public:

	TestScene(const std::string filepath) {
		
		//load the tilesheet 
		sheet.Load("blocks.png",24);
		//add the tilesheet
		tmap.AddTilesheet(sheet);
		//load a tilemap file
		tmap.LoadFromFile("map.txt");
		//create the entity manager for the scene
		entities = std::make_shared<EntityManager>();
		//create the merged box colliders for the tilemap
		tmap.CreateMergedColliders(entities);
		//create the player entity
		std::shared_ptr<Entity> player = std::make_shared<Entity>();
		playerptr = player;
		playerID = player->GetID();

		camera.offset = { 800 / 2, 600 / 2 }; // Where on the screen the target sits
		camera.rotation = 0.0f;
		camera.zoom = 1.0f;


		//add the player sprite sheet
		auto sprite = player->AddComponent<SpriteComponent>("Knight-Sheet.png", Rectangle{ 0, 0, 13.0f, 19.0f });
		sprite->priority = 6;
		//create the players box collider
		auto collider = player->AddComponent<BoxCollider2D>(Rectangle{ 0,0,13.0f, 19.0f });
		collider->debugDraw = true;
		collider->priority = 2;
		//create the player transform component and set layer above tiles
		auto transform = player->AddComponent<TransformComponent>(Vector2{ 100, 100 });
		transform->layer = 1;
		transform->priority = 0;
		//attach rigidbody for physics and input component
		auto rigid = player->AddComponent<RigidBody2D>();
		rigid->priority = 1;
		auto input = player->AddComponent<InputComponent>();

		auto feet = player->AddComponent<SensorComponent>(13.0, 19.0, SensorSide::Bottom);
		feet->priority = 3;
		feet->debugDraw = true;
		feet->SetSize(9.0f, 1.0f);
		feet->onCollision = [player, feet,input](Collider2D* c) {
			if(player->GetComponent<TransformComponent>()->velocity.y >= 0)
				input->OnSensorCollision(feet, c);
		};
	

		auto left = player->AddComponent<SensorComponent>(13.0, 19.0, SensorSide::Left);
		left->priority = 4;
		left->debugDraw = true;
		left->SetSize(1.0f, 10.0f);
		left->onCollision = [left, input](Collider2D* c) {
			input->OnSensorCollision(left, c);
			};
		auto right = player->AddComponent<SensorComponent>(13.0, 19.0, SensorSide::Right);
		right->priority = 5;
		right->debugDraw = true;
		right->SetSize(1.0f, 10.0f);
		right->onCollision = [right, input](Collider2D* c) {
			input->OnSensorCollision(right, c);
			};
		//add the player entity to the entity manager
		entities->AddEntity(player);
	};
	virtual ~TestScene() override {};
	virtual void Update(bool& isRunning, float dt = 0.0f) override {
		auto transform = playerptr->GetComponent<TransformComponent>();
		camera.target = { transform->position.x, transform->position.y };


		entities->Update(dt);
	};
	virtual void Draw() override {
		ClearBackground(SKYBLUE);

		BeginMode2D(camera);   

		Vector2 pos{ 0,0 };
		sheet.Draw(pos);
		tmap.Draw();
		entities->Draw();

		EndMode2D();           
	}
	virtual void HandleInput() override {
	};
};
