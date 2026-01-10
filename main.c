#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

// config
#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT SCREEN_WIDTH
#define CELL_SIZE 2
#define CHUNK_SIZE 32

// grid size based on cell size
#define COLS (SCREEN_WIDTH / CELL_SIZE)
#define ROWS (SCREEN_HEIGHT / CELL_SIZE)

#define CHUNK_COLS (CHUNK_SIZE / CELL_SIZE)
#define CHUNK_ROWS (CHUNK_SIZE / CELL_SIZE)

#define NUM_CHUNKS_X (COLS / CHUNK_COLS + 1)
#define NUM_CHUNKS_Y (ROWS / CHUNK_ROWS + 1)

typedef enum {
    MAT_AIR, MAT_SAND
}MaterialType;

char *grid;
int grainCount = 0;
unsigned int brushSize = 50;
unsigned int physicsSteps = 1;
bool showDebug = 1;

bool activeChunks[NUM_CHUNKS_X][NUM_CHUNKS_Y];
bool nextActiveChunks[NUM_CHUNKS_X][NUM_CHUNKS_Y];

bool fallRight = true;

void wakeChunk(int x, int y) {
    for (int cx = x - 1; cx <= x + 1; cx++) {
        for (int cy = y - 1; cy <= y + 1; cy++) {
            if (cx >= 0 && cx < NUM_CHUNKS_X && cy >= 0 && cy < NUM_CHUNKS_Y) {
                activeChunks[cx][cy] = true;
                nextActiveChunks[cx][cy] = true;
            }
        }
    }
}


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
                    if (grid[idx] == MAT_AIR) {
                        grid[idx] = MAT_SAND;
                        grainCount++;

                        //waking current and surrounding chunks
                        int cx = dx / CHUNK_COLS;
                        int cy = dy / CHUNK_ROWS;
                        wakeChunk(cx, cy);
                    }
                }
            }
        }
    }
}

void removeGrains() {
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
                    if (grid[idx] == MAT_SAND) {
                        grid[idx] = MAT_AIR;
                        grainCount--;

                        //waking current and surrounding chunks
                        int cx = dx / CHUNK_COLS;
                        int cy = dy / CHUNK_ROWS;
                        wakeChunk(cx, cy);
                    }
                }
            }
        }
    }
}

void randomPattern() {
    int totalCells = COLS * ROWS;
    int targetGrains = totalCells / 4;
    int i = 0;
    int attempts = 0; 
    int maxAttempts = targetGrains * 10;
    while (i < targetGrains && attempts < maxAttempts) {
        attempts++;
        int randx = rand() % COLS;
        int randy = rand() % ROWS;
        int idx = randy * COLS + randx;
        if(grid[idx] == MAT_AIR) {
            grid[idx] = MAT_SAND;
            grainCount++;
            i++;

            //waking current and surrounding chunks
            int cx = randx / CHUNK_COLS;
            int cy = randy / CHUNK_ROWS;
            wakeChunk(cx, cy);
        }
    }
}

void rotateRight() {
    char* newGrid = (char *)calloc(COLS * ROWS, sizeof(char));
    for (int y = 0; y < ROWS; y++) {
        for(int x = 0; x < COLS; x++){
            int src_index = (y * ROWS) + x;
            int dest_index = ((COLS - 1 - x) * ROWS) + y;

            newGrid[dest_index] = grid[src_index];
        }
    }
    free(grid);
    grid = newGrid;
    for (int x = 0; x < NUM_CHUNKS_X; x++){
        for (int y = 0; y < NUM_CHUNKS_Y; y++){
            wakeChunk(x, y);
        }
    }
}

void rotateLeft() {
    char* newGrid = (char *)calloc(COLS * ROWS, sizeof(char));
    for (int y = 0; y < ROWS; y++) {
        for(int x = 0; x < COLS; x++){
            int src_index = (y * ROWS) + x;
            int dest_index = (x * ROWS) + (ROWS - 1 - y);

            newGrid[dest_index] = grid[src_index];
        }
    }
    free(grid);
    grid = newGrid;
    for (int x = 0; x < NUM_CHUNKS_X; x++){
        for (int y = 0; y < NUM_CHUNKS_Y; y++){
            wakeChunk(x, y);
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


    //begin by waking up all chunks
    for (int x = 0; x < NUM_CHUNKS_X; x++){
        for (int y = 0; y < NUM_CHUNKS_Y; y++){
            wakeChunk(x, y);
        }
    }

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_UP)) physicsSteps++;

        if (IsKeyPressed(KEY_DOWN) && physicsSteps > 1) physicsSteps--;

        if (IsKeyPressed(KEY_R)) randomPattern();

        if (IsKeyPressed(KEY_D)) showDebug = !showDebug;

        if (IsKeyPressed(KEY_LEFT)) rotateLeft();
        if (IsKeyPressed(KEY_RIGHT)) rotateRight();

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            addGrains();
        }
        
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            removeGrains();
        }

        //UPDATE (cpu)
        for (int i = 0; i < physicsSteps; i++) {
            //clear next frame chunks
            memset(nextActiveChunks, 0, sizeof(nextActiveChunks));

            //loop through chunks
            for (int cy = NUM_CHUNKS_Y - 1; cy >= 0; cy--){
                for (int cx = 0; cx < NUM_CHUNKS_X; cx++){

                    //skip if not active
                    if (!activeChunks[cx][cy]) continue;

                    //calculating borders of current chunk
                    int startx =  cx * CHUNK_COLS;
                    int endx = startx + CHUNK_COLS;
                    int starty = cy * CHUNK_ROWS;
                    int endy = starty + CHUNK_ROWS;

                    //make sure to not go over window size
                    if (endx > COLS) endx = COLS;
                    if (endy > ROWS) endy = ROWS;

                    bool chunkMoved = false;

                    //standard physics loop, but only inside current chunk
                    for (int y = endy - 1; y >= starty; y--) {
                        if (y >= ROWS - 1) continue;

                        for (int x = startx; x < endx; x ++) {
                            int idx = y * COLS + x;

                            if (grid[idx] == MAT_SAND) {
                                int below = (y + 1) * COLS + x;
                                int belowLeft = (y + 1) * COLS + (x - 1);
                                int belowRight = (y + 1) * COLS + (x + 1);


                                // 1. below
                                if (grid[below] == MAT_AIR) {
                                    grid[below] = MAT_SAND;
                                    grid[idx] = MAT_AIR;
                                    chunkMoved = true;
                                    continue;
                                }
                                
                                if (!fallRight) {
                                    fallRight = !fallRight;
                                    // 1. below-left
                                    if (x > 0 && grid[belowLeft] == MAT_AIR) {
                                        grid[belowLeft] = MAT_SAND;
                                        grid[idx] = MAT_AIR;
                                        chunkMoved = true;
                                        continue;
                                    }

                                    // 2. below-right
                                    if (x < COLS -1 && grid[belowRight] == MAT_AIR) {
                                        grid[belowRight] = MAT_SAND;
                                        grid[idx] = MAT_AIR;
                                        chunkMoved = true;
                                        continue;
                                    }
                                }else{
                                    fallRight = !fallRight;
                                    if (x < COLS -1 && grid[belowRight] == MAT_AIR) {
                                        grid[belowRight] = MAT_SAND;
                                        grid[idx] = MAT_AIR;
                                        chunkMoved = true;
                                        continue;
                                    }
                                    
                                    // 2. below-left
                                    if (x > 0 && grid[belowLeft] == MAT_AIR) {
                                        grid[belowLeft] = MAT_SAND;
                                        grid[idx] = MAT_AIR;
                                        chunkMoved = true;
                                        continue;
                                    }

                                }
                                
                            }
                        }
                    }

                    if (chunkMoved) {
                        wakeChunk(cx, cy);
                    }
                }
            }

            //copy "next" to current
            memcpy(activeChunks, nextActiveChunks, sizeof(activeChunks));
        }

        //RENDER PREPARATION
        for (int i = 0; i < COLS * ROWS; i++) {
            if (grid[i] == MAT_SAND) {
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

        if (showDebug) {
            for (int cy = 0; cy < NUM_CHUNKS_Y; cy++) {
                for (int cx = 0; cx < NUM_CHUNKS_X; cx++) {
                    Color c = activeChunks[cx][cy] ? GREEN : RED;
                    if (activeChunks[cx][cy]) {
                        DrawRectangleLines(cx * CHUNK_SIZE, cy * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, c);
                    } else {
                        DrawRectangleLines(cx * CHUNK_SIZE, cy * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, Fade(RED, 0.1f));
                    }
                }
            }
        DrawFPS(10, 10);
        DrawText(TextFormat("Physics steps per frame: %d, simulated FPS: %d", physicsSteps, GetFPS() * physicsSteps), 10, 40, 20, GREEN);
        DrawText(TextFormat("Grains: %d", grainCount), SCREEN_WIDTH - 220, 10, 20, GREEN);

        }

        
        
        EndDrawing();
    }

    //CLEANING
    UnloadImage(sandImage);
    UnloadTexture(sandTexture);
    free(grid);

    CloseWindow();
    return 0;
}