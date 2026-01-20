#pragma once
#include <array>
#include "AEEngine.h"

// Define the dimensions of the game map
const int MAP_HEIGHT = 36;  // Was 18, now double 
const int MAP_WIDTH = 64;   // Was 32, now double 
/**
 * Loads a game level map from a text file into a 2D array.
 * Returns 1 on success, 0 on failure.
 */
int load_game_level(const char* filename, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr);

/**
 * Renders the game map using the provided mesh.
 * Draws colored squares for each solid tile (value of 1) in the array.
 */
void Load_map(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr,
    AEGfxVertexList* mesh,
    float cameraX, float cameraY);
