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

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetTransparency(1.0f);
	AEMtx33 scale, translate, transform;
	const int TILE_SIZE = 50;
	const int OUTLINE_THICKNESS = 4;

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (arr[i][j] != 0) {

				float x = (j * TILE_SIZE) - 800.0f + 25.0f;
				float y = 450.0f - (i * TILE_SIZE) - 25.0f;
				float screenX = x - cameraX;
				float screenY = y - cameraY;

				
				if (arr[i][j] == 1) {
					
					AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f);
				}
				else if (arr[i][j] == 2) {
					
					AEGfxSetColorToMultiply(0.0f, 1.0f, 1.0f, 1.0f);
				}
				else if (arr[i][j] == 3) {
					
					AEGfxSetColorToMultiply(0.5f, 1.0f, 0.0f, 1.0f);
				}
				else if (arr[i][j] == 4) {
					
					AEGfxSetColorToMultiply(1.0f, 0.8f, 0.0f, 1.0f);
				}
				else if (arr[i][j] == 5) {
					
					AEGfxSetColorToMultiply(1.0f, 0.0f, 1.0f, 1.0f);
				}
				else {
					
					AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
				}

				// Draw neon outline 
				AEMtx33Scale(&scale, (float)(TILE_SIZE + OUTLINE_THICKNESS * 2), (float)(TILE_SIZE + OUTLINE_THICKNESS * 2));
				AEMtx33Trans(&translate, screenX, screenY);
				AEMtx33Concat(&transform, &translate, &scale);
				AEGfxSetTransform(transform.m);
				AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

				// Draw black interior 
				AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
				AEMtx33Scale(&scale, (float)TILE_SIZE, (float)TILE_SIZE);
				AEMtx33Trans(&translate, screenX, screenY);
				AEMtx33Concat(&transform, &translate, &scale);
				AEGfxSetTransform(transform.m);
				AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
			}
		}
	}
}