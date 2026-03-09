#pragma once
#include <array>
#include "AEEngine.h"

// Define the dimensions of the game map
const int MAP_HEIGHT = 64;  // Was 18, now double 
const int MAP_WIDTH = 64;   // Was 32, now double 
const int TILE_SIZE = 50;
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
void Load_Map(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr,
    AEGfxVertexList* mesh,
    float cameraX, float cameraY);

void Spawn_Flag(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr);

struct CameraPos {
    float x = 0.0f;
    float y = 0.0f;
};
CameraPos moving_cursor(CameraPos camera);
void hover(float cameraX, float cameraY, AEGfxVertexList* mesh);