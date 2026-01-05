#include "raylib.h"

// config
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define CELL_SIZE 4

// grid size based on cell size
#define COLS (SCREEN_WIDTH / CELL_SIZE)
#define ROWS (SCREEN_HEIGHT / CELL_SIZE)

// 0 = empty, 1 = sand
int grid[COLS][ROWS] = {0};
int grainCount = 0;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sand simulator");
    SetTargetFPS(120);

    while (!WindowShouldClose()) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            
            int mouseGridX = (int)mouse.x / CELL_SIZE;
            int mouseGridY = (int)mouse.y / CELL_SIZE;
            
            int radiusInGrid = 50 / CELL_SIZE; 
            if (radiusInGrid < 1) radiusInGrid = 1;

            for (int y = -radiusInGrid; y <= radiusInGrid; y++) {
                for (int x = -radiusInGrid; x <= radiusInGrid; x++) {
                    
                    if (x*x + y*y <= radiusInGrid*radiusInGrid) {
                        int drawX = mouseGridX + x;
                        int drawY = mouseGridY + y;

                        if (drawX >= 0 && drawX < COLS && drawY >= 0 && drawY < ROWS) {
                            // checking if pixel is empty for grain counter
                            if (grid[drawX][drawY] == 0){
                                grid[drawX][drawY] = 1;
                                grainCount++;
                            }
                        }
                    }
                }
            }
        }
        
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            
            int mouseGridX = (int)mouse.x / CELL_SIZE;
            int mouseGridY = (int)mouse.y / CELL_SIZE;
            
            int radiusInGrid = 50 / CELL_SIZE; 
            if (radiusInGrid < 1) radiusInGrid = 1;

            for (int y = -radiusInGrid; y <= radiusInGrid; y++) {
                for (int x = -radiusInGrid; x <= radiusInGrid; x++) {
                    
                    if (x*x + y*y <= radiusInGrid*radiusInGrid) {
                        int drawX = mouseGridX + x;
                        int drawY = mouseGridY + y;

                        if (drawX >= 0 && drawX < COLS && drawY >= 0 && drawY < ROWS) {
                            //checking if pixel is full for grain counter
                            if (grid[drawX][drawY] == 1){
                                grid[drawX][drawY] = 0;
                                grainCount--;
                            }
                        }
                    }
                }
            }
        }

        for (int y = ROWS - 2; y >= 0; y--) {
            for (int x = 0; x < COLS; x++) {
                
                if (grid[x][y] == 1) { 
                    
                    if (grid[x][y+1] == 0) {
                        grid[x][y+1] = 1;
                        grid[x][y] = 0;
                    } 
                    else if (x > 0 && grid[x-1][y+1] == 0) {
                        grid[x-1][y+1] = 1;
                        grid[x][y] = 0;
                    }
                    else if (x < COLS - 1 && grid[x+1][y+1] == 0) {
                        grid[x+1][y+1] = 1;
                        grid[x][y] = 0;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < COLS; x++) {
                if (grid[x][y] == 1) {
                    DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, YELLOW);
                }
            }
        }

        DrawFPS(10, 10);
        
        DrawText(TextFormat("Grains: %d", grainCount), SCREEN_WIDTH - 220, 10, 20, GREEN);
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}