#include "./game.h"
#include <random>
#include <ctime>
#include "./colors.h"

Game::Game() {
	srand(static_cast<unsigned>(time(0)));
	grid = Grid();
	blocks = GetAllBlocks();
	currentBlock = GetRandomBlock();
	nextBlock = GetRandomBlock();
	gameOver = false;
	score = 0;
	moveCooldown = 0;
	font = LoadFontEx("Font/Tetris.ttf", 64, 0, 0);
	gameStartTime = GetTime();
	currentTime = 0;
	speed = 0;
}

Block Game::GetRandomBlock() {
	if (blocks.empty()) {
		blocks = GetAllBlocks();
	}
	int randomIndex = rand() % blocks.size();
	Block block = blocks[randomIndex];
	blocks.erase(blocks.begin() + randomIndex);
	return block;
}

std::vector<Block> Game::GetAllBlocks() {
	return { IBlock(), JBlock(), LBlock(), OBlock(), SBlock(), TBlock(), ZBlock() };
}

bool Game::EventTriggered(double interval) {
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval) {
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

void Game::UpdateEventInterval() {
	if (gameOver) {
		return;
	}

	double elapsedTime = GetTime() - gameStartTime;

	// Time-based scaling from 1 to 100
	double minSpeed = 1.0;  
	double maxSpeed = 100.0;  
	double difficultyFactor = elapsedTime / 1200.0;  

	speed = minSpeed + (maxSpeed - minSpeed) * std::min(1.0, difficultyFactor); // Scale between 1 and 100

	eventInterval = 1.0 / speed; 

	// Score-based scaling
	if (score > 0) {
		eventInterval *= pow(0.995, score / 100); // 1% speed increase for every 100 points
	}

	if (eventInterval < 0.05) {
		eventInterval = 0.05; 
	}
}

void Game::Draw() {
	DrawTextEx(font, "TETRIS", { 11,15 }, 40, 2, PURPLE);
	grid.Draw();
	DrawNextBlock();
	DrawScore();
	DrawTime();
	DrawSpeed();

	Color darkBlue = { 75, 75, 100, 255 };
	ClearBackground(darkBlue);

	UpdateEventInterval();

	if (EventTriggered(eventInterval)) {
		MoveBlockDown();
	}

	if (gameOver) {
		if (static_cast<int>(GetTime()) % 2 == 0) { // Check every second
			DrawTextEx(font, "GAME OVER", { 215,248 }, 40, 2, WHITE);
		}
	}
}

void Game::DrawNextBlock() {
	DrawTextEx(font, "Next Block", { 14,75 }, 25, 2, WHITE);
	DrawRectangleRounded({ 12, 110, 170, 180 }, 0.3, 6, gray);
	currentBlock.Draw(195, 10);
	switch (nextBlock.id) {
	case 3:
		nextBlock.Draw(-50, 180);
		break;
	case 4:
		nextBlock.Draw(-50, 165);
		break;
	default:
		nextBlock.Draw(-40, 165);
		break;
	}
}

void Game::DrawScore() {
	char scoreText[10];
	sprintf(scoreText, "%d", score);
	Vector2 textSize = MeasureTextEx(font, scoreText, 25, 2);
	DrawTextEx(font, "Score", { 50,305 }, 25, 2, WHITE);
	DrawRectangleRounded({ 12, 335, 170, 60 }, 0.4, 6, gray);
	DrawTextEx(font, scoreText, { 12 + (170 - textSize.x) / 2, 353 }, 25, 2, WHITE);
}

void Game::DrawTime() {
	if (!gameOver) {
		currentTime = GetTime() - gameStartTime;
	}

	int minutes = 0;
	int seconds = 0;

	minutes = static_cast<int>(currentTime) / 60;
	seconds = static_cast<int>(currentTime) % 60;

	char timeText[10];
	sprintf(timeText, "%02d:%02d", minutes, seconds);
	Vector2 textSize = MeasureTextEx(font, timeText, 25, 2);
	DrawTextEx(font, "Time", { 57,405 }, 25, 2, WHITE);
	DrawRectangleRounded({ 12, 435, 170, 60 }, 0.4, 6, gray);

	DrawTextEx(font, timeText, { 12 + (170 - textSize.x) / 2, 453 }, 25, 2, WHITE);
}

void Game::DrawSpeed() {
	DrawTextEx(font, "Speed", { 50,505 }, 25, 2, WHITE);
	DrawRectangleRounded({ 12, 535, 170, 60 }, 0.4, 6, gray);

	char multText[10];
	sprintf(multText, "%.2f", speed);
	Vector2 textSize = MeasureTextEx(font, multText, 25, 2);
	DrawTextEx(font, multText, { 57 + (75 - textSize.x) / 2, 555 }, 25, 2, WHITE);
}

void Game::HandleInput() {
	static int lastDownPressTime = 0; 
	static int currentFrame = 0;
	static bool isKeyDownPreviously = false;
	static int keyDownDuration = 0; 
	static int lastKeyDownTime = 0;

	currentFrame++;

	if (gameOver) {
		if (GetKeyPressed() != 0) { 
			gameOver = false;
			Reset();
		}
		return; 
	}

	if (moveCooldown > 0) {
		moveCooldown--;
		return;
	}

	if (IsKeyDown(KEY_LEFT)) {
		MoveBlockLeft();
		moveCooldown = 5;
	}

	if (IsKeyDown(KEY_RIGHT)) {
		MoveBlockRight();
		moveCooldown = 5;
	}

	if (IsKeyDown(KEY_UP)) {
		RotateBlock();
		moveCooldown = 7;
	}

	if (IsKeyDown(KEY_DOWN)) {
		int timeSinceLastPress = currentFrame - lastDownPressTime;

		if (timeSinceLastPress > 0 && timeSinceLastPress <= 10) { // instant block drop
			if (!isKeyDownPreviously) {
				while (!IsBlockOutside() && BlockFits()) {
					currentBlock.Move(1, 0);
					UpdateScore(0, 1);
				}
				currentBlock.Move(-1, 0);
				LockBlock();
				moveCooldown = 3;
				isKeyDownPreviously = true; // Prevent re-triggering double-tap immediately
			}
		}
		else if (keyDownDuration > 10) { // hold down drop faster
			MoveBlockDown();
			UpdateScore(0, 1);
			moveCooldown = 1;
		}
		else { // one tap
			if (!isKeyDownPreviously) {
				MoveBlockDown();
				UpdateScore(0, 1);
				moveCooldown = 3;
				lastDownPressTime = currentFrame;
				isKeyDownPreviously = true;
			}
		}
		keyDownDuration++;
	}
	else {
		isKeyDownPreviously = false;
		keyDownDuration = 0;
	}
}

void Game::MoveBlockLeft() {
	if (!gameOver) {
		currentBlock.Move(0, -1);
		if (IsBlockOutside() || BlockFits() == false) {
			currentBlock.Move(0, 1);
		}
	}
}

void Game::MoveBlockRight() {
	if (!gameOver) {
		currentBlock.Move(0, 1);
		if (IsBlockOutside() || BlockFits() == false) {
			currentBlock.Move(0, -1);
		}
	}
}

void Game::MoveBlockDown() {
	if (!gameOver) {
		currentBlock.Move(1, 0);
		if (IsBlockOutside() || BlockFits() == false) {
			currentBlock.Move(-1, 0);
			LockBlock();
		}
	}
	
}

bool Game::IsBlockOutside() {
	std::vector<Position> tiles = currentBlock.GetCellPositions();
	for (Position item : tiles) {
		if (grid.isCellOutside(item.row, item.column)) {
			return true;
		}
	}
	return false;
}

void Game::RotateBlock() {
	if (!gameOver) {
		currentBlock.Rotate();
		if (IsBlockOutside() || BlockFits() == false) {
			currentBlock.UndoRotation();
		}
	}
}

void Game::LockBlock() {
	std::vector<Position> tiles = currentBlock.GetCellPositions();
	for (Position item : tiles) {
		grid.grid[item.row][item.column] = currentBlock.id;
	}
	currentBlock = nextBlock;
	if (BlockFits() == false) {
		gameOver = true;
		return;
	}
	nextBlock = GetRandomBlock();
	int rowsCleared = grid.ClearFullRows();
	UpdateScore(rowsCleared, 0);
}

bool Game::BlockFits() {
	std::vector<Position> tiles = currentBlock.GetCellPositions();
	for (Position pos : tiles) {
		if (pos.row < 0 || pos.row >= grid.numRows ||
			pos.column < 0 || pos.column >= grid.numCols ||
			grid.grid[pos.row][pos.column] != 0) {
			return false; 
		}
	}
	return true;
}

void Game::Reset(){
	grid.Initialize();
	blocks = GetAllBlocks();
	currentBlock = GetRandomBlock();
	nextBlock = GetRandomBlock();
	score = 0;
	gameStartTime = GetTime(); // reset time
	eventInterval = 1;
	speed = 0;
}

void Game::UpdateScore(int linesCleared, int moveDownPoints){
	switch (linesCleared) {
	case 1:
		score += 100;
		break;
	case 2:
		score += 300;
		break;
	case 3:
		score += 500;
		break;
	default:
		break;
	}

	score += moveDownPoints;
}
