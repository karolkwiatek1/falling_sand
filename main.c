#include "raylib.h"
#include <stdlib.h>

// config
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define CELL_SIZE 1

// grid size based on cell size
#define COLS (SCREEN_WIDTH / CELL_SIZE)
#define ROWS (SCREEN_HEIGHT / CELL_SIZE)

// 0 = empty, 1 = sand
char *grid;
int grainCount = 0;
unsigned int brushSize = 50;
unsigned int physicsSteps = 1;

void addGrains() {
    Vector2 mouse = GetMousePosition();

    int mx = (int)mouse.x / CELL_SIZE;
    int my = (int)mouse.y / CELL_SIZE;

    int r = brushSize / CELL_SIZE;
    if (r < 1) r = 1;

    for (int y = -r; y <= r; y++){
        for (int x = -r; x <= r; x++){
            if (x*x + y*y <= r*r) {
                int dx = mx + x;
                int dy = my + y;
                if (dx >= 0 && dx < COLS && dy >= 0 && dy < ROWS) {
                    //index in 1d array = y * width + x;
                    int idx = dy * COLS + dx;
                    if (grid[idx] == 0) {
                        grid[idx] = 1;
                        grainCount++;
                    }
                }
            }
        }
    }
}

void removeGraind() {
    Vector2 mouse = GetMousePosition();

    int mx = (int)mouse.x / CELL_SIZE;
    int my = (int)mouse.y / CELL_SIZE;

    int r = brushSize / CELL_SIZE;
    if (r< 1) r = 1;

    for (int y = -r; y <= r; y++){
        for (int x = -r; x <= r; x++){
            if (x*x + y*y <= r*r) {
                int dx = mx + x;
                int dy = my + y;
                if (dx >= 0 && dx < COLS && dy >= 0 && dy < ROWS) {
                    //index in 1d array = y * width + x;
                    int idx = dy * COLS + dx;
                    if (grid[idx] == 1) {
                        grid[idx] = 0;
                        grainCount--;
                    }
                }
            }
        }
    }
}


int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sand simulator");
    SetTargetFPS(60);

    //allocating memory for the grid (using flat array for performance)
    grid = (char *)calloc(COLS * ROWS, sizeof(char));

    Image sandImage = GenImageColor(COLS, ROWS, BLACK);
    Texture2D sandTexture = LoadTextureFromImage(sandImage);

    Color *pixels = (Color *)sandImage.data;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_UP)) physicsSteps++;

        if (IsKeyPressed(KEY_DOWN) && physicsSteps > 1) physicsSteps--;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            addGrains();
        }
        
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            removeGraind();
        }

        //UPDATE (cpu)
        for (int i = 0; i < physicsSteps; i++) {
            for (int y = ROWS - 2; y >= 0; y--) {
                for (int x = 0; x < COLS; x ++) {
                    int idx = y * COLS + x;

                    if (grid[idx] == 1) {
                        int below = (y + 1) * COLS + x;
                        int belowLeft = (y + 1) * COLS + (x - 1);
                        int belowRight = (y + 1) * COLS + (x + 1);

                        // 1. below
                        if (grid[below] == 0) {
                            grid[below] = 1;
                            grid[idx] = 0;
                        }
                        
                        // 2. below-left
                        else if (x > 0 && grid[belowLeft] == 0) {
                            grid[belowLeft] = 1;
                            grid[idx] = 0;
                        }

                        // 2. below-right
                        else if (x < COLS -1 && grid[belowRight] == 0) {
                            grid[belowRight] = 1;
                            grid[idx] = 0;
                        }
                    }
                }
            }
        }

        //RENDER PREPARATION
        for (int i = 0; i < COLS * ROWS; i++) {
            if (grid[i] == 1) {
                pixels[i] = YELLOW;
            } else {
                pixels[i] = BLACK;
            }
        }

        UpdateTexture(sandTexture, pixels);

        //DRAW
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(sandTexture, (Rectangle){0, 0, COLS, ROWS}, (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, (Vector2){0, 0}, 0.0f, WHITE);

        DrawFPS(10, 10);
        DrawText(TextFormat("Physics steps per frame: %d, simulated FPS: %d", physicsSteps, GetFPS() * physicsSteps), 10, 40, 20, GREEN);
        DrawText(TextFormat("Grains: %d", grainCount), SCREEN_WIDTH - 220, 10, 20, GREEN);
        
        EndDrawing();
    }

    //CLEANING
    UnloadImage(sandImage);
    UnloadTexture(sandTexture);
    free(grid);

    CloseWindow();
    return 0;
}