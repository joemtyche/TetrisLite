#pragma once
#include <vector>
#include <raylib.h>

class Grid {
public:
	Grid();
	void Initialize();
	void Print();
	void Draw();
	bool isCellOutside(int row, int column);
	bool isCellEmpty(int row, int column);
	int ClearFullRows();
	int numRows;
	int numCols;
	int grid[20][10];
private:
	bool isRowFull(int row);
	void ClearRow(int row);
	void MoveRowDown(int row, int numRows);
	int cellSize;

	std::vector<Color> colors;
};