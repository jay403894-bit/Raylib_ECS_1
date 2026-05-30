#include "Scenes.h"
#include <memory>
	StartMenu::StartMenu()
	{
		fnt = FontManager::Get("telegrama_raw");
	}

	void StartMenu::Update(bool& isRunning, float dt)
	{
		if (quit)
			isRunning = false;
	}

	void StartMenu::Draw()
	{
		if (fnt)
		{
			ClearBackground(DARKBLUE);
			DrawRectangleRounded(titleBG, 0.3, 6, LIGHTGRAY);
			DrawTextEx(*fnt, "StartMenuExample", { 90, 25 }, 32, 2, BLACK);

			DrawRectangleRounded(menuBG, 0.3, 6, BLACK);
			// Draw buttons
			// Draw the buttons, highlighting the selected one based on menuIndex
			DrawRectangleRounded(newGameButton, 0.3, 6, (menuIndex == 0) ? DARKGRAY : LIGHTGRAY);
			DrawRectangleRounded(continueButton, 0.3, 6, (menuIndex == 1) ? DARKGRAY : LIGHTGRAY);
			DrawRectangleRounded(optionsButton, 0.3, 6, (menuIndex == 2) ? DARKGRAY : LIGHTGRAY);
			DrawRectangleRounded(quitButton, 0.3, 6, (menuIndex == 3) ? DARKGRAY : LIGHTGRAY);


			// Draw text on buttons
			DrawTextEx(*fnt, "New Game", { 180, 305 }, 32, 2, BLACK);
			DrawTextEx(*fnt, "Continue", { 180, 345 }, 32, 2, BLACK);
			DrawTextEx(*fnt, "Options", { 180, 388 }, 32, 2, BLACK);
			DrawTextEx(*fnt, "Quit", { 180, 430 }, 32, 2, BLACK);

			// Draw the cursor (red circle)
		//	DrawCircle(menuX, menuY, 12, RED);
		}
		else
		{
			std::cerr << "Error: Failed to load font" << std::endl;
		}
	}

	void StartMenu::HandleInput()
	{
		// Keyboard input handling (W, S)
		int keyPressed = GetKeyPressed();
		switch (keyPressed)
		{
		case KEY_S:
			if (menuIndex == 3)
				menuIndex = 0;
			else
				menuIndex++;
			break;

		case KEY_W:
			if (menuIndex == 0)
				menuIndex = 3;
			else
				menuIndex--;
			break;

		case KEY_ESCAPE:
			QuitPressed();
			break;

		case KEY_DOWN:
			if (menuIndex == 3)
				menuIndex = 0;
			else
				menuIndex++;
			break;

		case KEY_UP:
			if (menuIndex == 0)
				menuIndex = 3;
			else
				menuIndex--;
			break;

		case KEY_ENTER:
			switch (menuIndex) {
			case 0:
				NewGamePressed();
				break;
			case 1: 
				ContinuePressed();
				break;
			case 2:
				OptionsPressed();
				break;
			case 3:
				QuitPressed();
				break;
			}
			break;

		default:
			break;
		}

		// Mouse input handling
		Vector2 mousePosition = { (float)GetMouseX(), (float)GetMouseY() };

		// Check for mouse hover over buttons
		if (CheckCollisionPointRec(mousePosition, newGameButton))
		{
			menuIndex = 0;
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				// Action for New Game button
				NewGamePressed();
			}
		}
		else if (CheckCollisionPointRec(mousePosition, continueButton))
		{
			menuIndex = 1;
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				// Action for Continue button
				ContinuePressed();
			}
		}
		else if (CheckCollisionPointRec(mousePosition, optionsButton))
		{
			menuIndex = 2;
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				// Action for Options button
				OptionsPressed();
			}
		}
		else if (CheckCollisionPointRec(mousePosition, quitButton))
		{
			menuIndex = 3;
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				// Action for Quit button
				QuitPressed();
			}
		}
	}

	StartMenu::~StartMenu() {}

	void StartMenu::NewGamePressed()
	{
		std::cout << "New Game button pressed!" << std::endl;
	    SceneManager::PushScene(std::make_unique<TestScene>("untitled.tmj"));
		//SceneManager::PushScene(std::make_unique<MapScene>("untitled.tmj"));

	}


	void StartMenu::ContinuePressed()
	{
		std::cout << "Continue button pressed!" << std::endl;

	}

	void StartMenu::OptionsPressed()
	{
		std::cout << "Options button pressed!" << std::endl;
	}

	void StartMenu::QuitPressed()
	{
		std::cout << "Quit button pressed!" << std::endl;
		quit = true;
	}
