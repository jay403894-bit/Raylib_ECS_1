#pragma once
#include <random>
#include "grid.h"
#include "Blocks.hpp"
#include "Colors.h"
#include <iostream>

class Game
{
public:
	int score;
	bool gameOver;
	Grid grid;
	Music music;

	Game();
	~Game();
	std::vector<Block> GetAllBlocks();
	void Draw();
	void HandleInput();
	void MoveBlockDown();


private:
	Block GetRandomBlock();
	void MoveBlockLeft();
	void MoveBlockRight();
	void Reset();
	bool IsBlockOutside();
	void RotateBlock();
	void LockBlock();
	bool BlockFits();
	void UpdateScore(int linesCleared, int moveDownPoints);

	Sound rotateSound;
	Sound clearSound;
	Block currentBlock;
	Block nextBlock;
	std::vector<Block> blocks;
};

