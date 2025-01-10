#pragma once
#include "grid.h"
#include "blocks.cpp"

class Game {
public:
	Game();
	void Draw();
	void HandleInput();
	void MoveBlockDown();
	double eventInterval;
	void UpdateEventInterval();
private:
	Block GetRandomBlock();
	std::vector<Block> GetAllBlocks();
	void MoveBlockLeft();
	void MoveBlockRight();
	bool IsBlockOutside();
	void RotateBlock();
	void LockBlock();
	bool BlockFits();
	void Reset();
	void UpdateScore(int linesCleared, int moveDownPoints);
	void DrawNextBlock();
	void DrawScore();
	void DrawTime();
	void DrawSpeed();
	int score;
	bool gameOver;
	Font font;

	double lastUpdateTime;
	bool EventTriggered(double interval);
	double speed;
	
	double gameStartTime;
	double currentTime;
	int moveCooldown;
	std::vector<Block> blocks;
	Block currentBlock;
	Block nextBlock;
	Grid grid;
};
