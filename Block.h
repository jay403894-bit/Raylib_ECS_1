#pragma once
#include <vector>
#include <unordered_map>
#include "Position.h"
#include "Colors.h"

class Block {
public:
	int id;
	std::unordered_map<int, std::vector<Position>> cells;
	int rowOffset;
	int columnOffset;
	Block();
	void Draw(int offsetX, int offsetY);
	void Move(int rows, int columns);
	void Rotate();
	void UndoRotation();
	std::vector<Position> GetCellPositions();
private:
	std::vector<Color> colors;
	int cellSize;
	int rotationState;
};