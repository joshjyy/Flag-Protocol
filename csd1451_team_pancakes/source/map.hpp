#pragma once
#include <array>
#include "AEEngine.h"

// Define the dimensions of the game map
const int MAP_HEIGHT = 18; // Number of rows in the map
const int MAP_WIDTH = 32;  // Number of columns in the map

/**
 * Loads a game level map from a text file into a 2D array.
 * Returns 1 on success, 0 on failure.
 */
int load_game_level(const char* filename, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr);

/**
 * Renders the game map using the provided mesh.
 * Draws colored squares for each solid tile (value of 1) in the array.
 */
void Load_map(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr, AEGfxVertexList* mesh);

/**
 * Creates and returns a square mesh for rendering map tiles.
 * Creates a 1x1 unit square centered at origin using two triangles.
 * Color parameter is in 0xAARRGGBB format (default: red 0xFFFF0000).
 * Returns pointer to the created mesh.
 */
AEGfxVertexList* CreateMapMesh(unsigned int color = 0xFFFF0000);