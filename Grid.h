#pragma once
#include <vector>
#include <raylib.h>
#include "Colors.h"

class Grid {
public:
	Grid();
	int grid[20][10];
	void Initialize();
	void Print();
	void Draw();
	bool IsCellOutside(int row, int column);
	bool IsCellEmpty(int row, int column);
	int ClearFullRows();
private:
	bool IsRowFull(int row);
	void ClearRow(int row);
	void MoveRowDown(int row, int numRows);
	int numRows;
	int numCols;
	int cellSize;
	std::vector<Color> colors;
};