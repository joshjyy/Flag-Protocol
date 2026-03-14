#pragma once
#include <array>
#include "AEEngine.h"

// Define the dimensions of the game map
const int MAP_HEIGHT = 64;  // Was 18, now double 
const int MAP_WIDTH = 64;   // Was 32, now double 
extern int TILE_SIZE;
extern std::array<std::array<int, MAP_WIDTH + 2>, MAP_HEIGHT + 2> newMap;

/**
 * Loads a game level map from a text file into a 2D array.
 * Returns 1 on success, 0 on failure.
 */
int Load_Game_Level(const char* filename, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr);

/**
 * Saves a game level map from a 2D array into a text file.
 */
void Save_Game_Level(const char* filename, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr);

/**
 * Renders the game map using the provided mesh.
 * Draws colored squares for each solid tile (value of 1) in the array.
 */
void Load_Map(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr, float cameraX, float cameraY);

void PrintBlock(int choice, float d, float screenX, float screenY, AEGfxVertexList* mesh);

void Spawn_Flag(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr);

struct CameraPos {
    float x = 0.0f;
    float y = 0.0f;
};
CameraPos moving_cursor(CameraPos camera, int TILE_SIZE);
void hover(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr, float cameraX, float cameraY, AEGfxVertexList* mesh, int current_tile);
void zoom(int& TILE_SIZE);
void floodfill(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr, int x, int y);
bool HasHole(std::array<std::array<int, MAP_WIDTH + 2>, MAP_HEIGHT + 2>& newMap);
