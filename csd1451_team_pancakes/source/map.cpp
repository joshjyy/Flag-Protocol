#include "AEEngine.h"
#include <iostream>
#include "map.hpp"
#include <fstream>
#include <array>

/* 
 * Loads a game level map from a text file into a 2D array.
 * Opens the specified file and reads MAP_HEIGHT x MAP_WIDTH integers.
 * Each integer represents a tile type (0 = empty, 1 = solid).
 * Returns 1 on success, 0 on failure (file not found or invalid format).
 * Prints error messages if file cannot be opened or data is incomplete.
 */
int Load_Game_Level(const char* filename, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr) {
	// Create input file stream and attempt to open the file
	std::ifstream map_file(filename);

	// Check if file opened successfully
	if (!map_file.is_open()) {
		std::cout << "Error: Could not open file " << filename << std::endl;
		return 0; // Return failure
	}

	// Loop through each row of the map
	for (int i = 0; i < MAP_HEIGHT; i++) {
		// Loop through each column of the map
		for (int j = 0; j < MAP_WIDTH; j++) {
			// Read integer from file into array, check for read errors
			if (!(map_file >> arr[i][j])) {
				std::cout << "Error: Failed to read at [" << i << "][" << j << "]" << std::endl;
				map_file.close(); // Close file before returning
				return 0; // Return failure
			}
		}
	}

	// Close the file after successful read
	map_file.close();
	std::cout << "Map loaded successfully!" << std::endl;
	return 1; // Return success
}


/**
 * Renders the game map by drawing colored squares for each solid tile.
 * Loops through the 2D array and draws a 50x50 pixel square for each tile marked as 1.
 * Uses transformation matrices to position and scale each tile correctly on screen.
 * Coordinates are calculated to center the map in a 1600x900 window.
 * Empty tiles (0) are skipped and not rendered.
 */
 /**
  * Renders the game map by drawing colored squares for each tile.
  * Different tile values are rendered with different colors:
  * 0 = empty (not drawn)
  * 1 = red (walls/obstacles)
  * 2 = blue (water)
  * 3 = green (grass)
  * 4 = yellow (sand)
  * 5 = purple (special tiles)
  */
void Load_Map(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr,
	AEGfxVertexList* mesh,
	float cameraX, float cameraY) {
	// Set rendering mode to use vertex colors
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	// Set transparency to fully opaque (1.0 = no transparency)
	AEGfxSetTransparency(1.0f);
	// Declare transformation matrices for scaling, translation, and final transform
	AEMtx33 scale, translate, transform;
	// Define the size of each tile in pixels
	const int TILE_SIZE = 50;

	// Loop through each row of the map
	for (int i = 0; i < MAP_HEIGHT; i++) {
		// Loop through each column of the map
		for (int j = 0; j < MAP_WIDTH; j++) {
			// Skip empty tiles (value of 0)
			if (arr[i][j] != 0) {
				// Set color based on tile type using if-else statements
				if (arr[i][j] == 1) {
					// Red - walls/obstacles
					AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);
				}
				else if (arr[i][j] == 2) {
					// Blue - water
					AEGfxSetColorToMultiply(0.0f, 0.0f, 1.0f, 1.0f);
				}
				else if (arr[i][j] == 3) {
					// Green - grass
					AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f);
				}
				else if (arr[i][j] == 4) {
					// Yellow - sand
					AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f);
				}
				else if (arr[i][j] == 5) {
					// Purple - special tiles
					AEGfxSetColorToMultiply(0.5f, 0.0f, 0.5f, 1.0f);
				}
				else {
					// White - unknown tile type
					AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
				}

				// Calculate X position: column * tile size - half window width + half tile
				float x = (j * TILE_SIZE) - 800.0f + 25.0f;
				// Calculate Y position: half window height - row * tile size - half tile
				float y = 450.0f - (i * TILE_SIZE) - 25.0f;

				//Convert world position to screen position
				float screenX = x - cameraX;
				float screenY = y - cameraY;

				// Create scale matrix to resize mesh from 1x1 to TILE_SIZE x TILE_SIZE
				AEMtx33Scale(&scale, (float)TILE_SIZE, (float)TILE_SIZE);
				// Create translation matrix to move tile to calculated position
				AEMtx33Trans(&translate, screenX, screenY);
				// Combine transformations: translate * scale (scale first, then move)
				AEMtx33Concat(&transform, &translate, &scale);
				// Apply the transformation matrix to the rendering pipeline
				AEGfxSetTransform(transform.m);
				// Draw the mesh as triangles at the transformed position
				AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
			}
		}
	}
}