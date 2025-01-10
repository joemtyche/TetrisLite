#include <raylib.h>
#include "../game.h"
#include <iostream>

int main() {
    
    InitWindow(500, 615, "Tetris Lite");
    SetTargetFPS(60);

    Game game = Game();

    game.eventInterval = 1;

    while (WindowShouldClose() == false) {
        game.HandleInput();
        
        game.Draw();

        EndDrawing();
    }

    CloseWindow();
}