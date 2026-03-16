#pragma once
#include <array>
#include "AEEngine.h"
#include <vector>

// Define the dimensions of the game map
extern int MAP_HEIGHT;
extern int MAP_WIDTH;
extern int TILE_SIZE;
extern std::vector<std::vector<int>> newMap;
/**
 * Loads a game level map from a text file into a 2D array.
 * Returns 1 on success, 0 on failure.
 */
int Load_Game_Level(const char* filename, std::vector<std::vector<int>>& arr);

/**
 * Saves a game level map from a 2D array into a text file.
 */
void Save_Game_Level(const char* filename, std::vector<std::vector<int>>& arr);

/**
 * Renders the game map using the provided mesh.
 * Draws colored squares for each solid tile (value of 1) in the array.
 */
void Load_Map(std::vector<std::vector<int>>& arr, float cameraX, float cameraY);

void PrintBlock(int choice, float d, float screenX, float screenY, AEGfxVertexList* mesh);

void Spawn_Flag(std::vector<std::vector<int>>& arr);

struct CameraPos {
    float x = 0.0f;
    float y = 0.0f;
};
CameraPos moving_cursor(CameraPos camera);
void zoom();
void floodfill(std::vector<std::vector<int>>& arr, int x, int y);
bool HasHole();
