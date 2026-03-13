#include "AEEngine.h"
#include <iostream>
#include "map.hpp"
#include <fstream>
#include <array>
#include <cstdlib>
#include <random>
#include "utils.hpp"  

std::random_device rd;
std::mt19937 gen(rd());	
int TILE_SIZE = 50;
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
 * Saves a game level map from a 2D array into a text file.
 * Opens the specified file and writes MAP_HEIGHT x MAP_WIDTH integers.
 * Each integer represents a tile type (0 = empty, 1-5 = different tiles).
 **/
void Save_Game_Level(const char* filename, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr) {
	// Create output file stream
	std::ofstream map_file(filename);

	// Check if file opened successfully
	if (!map_file.is_open()) {
		std::cout << "Error: Could not save file " << filename << std::endl;
		return;
	}

	// Loop through each row of the map
	for (int i = 0; i < MAP_HEIGHT; i++) {
		// Loop through each column of the map
		for (int j = 0; j < MAP_WIDTH; j++) {
			// Write each integer to file
			map_file << arr[i][j];

			// Add space between numbers, but not at the end of row
			if (j < MAP_WIDTH - 1) {
				map_file << " ";
			}
		}
		// New line at the end of each row
		map_file << std::endl;
	}

	// Close the file
	map_file.close();
	std::cout << "Map saved successfully to " << filename << "!" << std::endl;
}

/***
 * Renders the game map by drawing colored squares for each solid tile.
 * Loops through the 2D array and draws a 50x50 pixel square for each tile marked as 1.
 * Uses transformation matrices to position and scale each tile correctly on screen.
 * Coordinates are calculated to center the map in a 1600x900 window.
 * Empty tiles (0) are skipped and not rendered.
 */
 /***
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
	
	float cameraX, float cameraY) {
	

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (arr[i][j] != 0 && arr[i][j] != 5) {
				float x = (j * TILE_SIZE) - 800.0f + 25.0f;
				float y = 450.0f - (i * TILE_SIZE) - 25.0f;
				float screenX = x - cameraX;
				float screenY = y - cameraY;
				PrintBlock(arr[i][j], 1.0f, screenX, screenY, meshes.square);
			}
		}
	}
	
}

void PrintBlock(int choice, float d, float screenX, float screenY, AEGfxVertexList* mesh) {
	float r = 0.0f, g = 0.0f, b = 0.0f;
	if (choice == 1) { r = 0.0f; g = 1.0f; b = 0.0f; }  // green blocks
	else if (choice == 2) { r = 0.5f; g = 0.0f; b = 1.0f; }  // purple goal
	else if (choice == 3) { r = 1.0f; g = 0.0f; b = 0.0f; }  // red
	else if (choice == 4) { r = 0.0f; g = 1.0f; b = 1.0f; }  // lime flag
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetTransparency(1.0f);
	AEMtx33 scale, translate, transform;
	const int EDGE = 4; 
	
	AEGfxSetColorToMultiply(r * 0.2f, g * 0.2f, b * 0.2f, d);
	AEMtx33Scale(&scale, (float)(TILE_SIZE + EDGE), (float)(TILE_SIZE + EDGE));
	AEMtx33Trans(&translate, screenX + EDGE * 0.5f, screenY - EDGE * 0.5f);
	AEMtx33Concat(&transform, &translate, &scale);
	AEGfxSetTransform(transform.m);
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

	// ── STEP 2: Draw bright top-left highlight (slightly bigger) ──
	AEGfxSetColorToMultiply(r, g, b, d);
	AEMtx33Scale(&scale, (float)(TILE_SIZE + EDGE), (float)(TILE_SIZE + EDGE));
	AEMtx33Trans(&translate, screenX - EDGE * 0.5f, screenY + EDGE * 0.5f);
	AEMtx33Concat(&transform, &translate, &scale);
	AEGfxSetTransform(transform.m);
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

	// ── STEP 3: Draw main block face (base color darkened) ──
	AEGfxSetColorToMultiply(r * 0.35f, g * 0.35f, b * 0.35f, d);
	AEMtx33Scale(&scale, (float)TILE_SIZE, (float)TILE_SIZE);
	AEMtx33Trans(&translate, screenX, screenY);
	AEMtx33Concat(&transform, &translate, &scale);
	AEGfxSetTransform(transform.m);
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

void Spawn_Flag(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr) {
	struct Tile { int row, col; };
	
	int count = 0;
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (arr[i][j] == 5) {
				count++;
			}
		}
	}
	if (count == 0) {
		std::cout << "No flag tiles found!\n";
		return;
	}

	Tile* found = new Tile[count];
	int flagloc = 0;
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (arr[i][j] == 5) {
				found[flagloc].row = i;
				found[flagloc].col = j;
				flagloc++;
			}
		}
	}

	srand((unsigned int)time(0));
	int index = rand() % count;

	int row = found[index].row;
	int col = found[index].col;

	std::cout << "Flag spawned at Row: " << row << " Col: " << col << "\n";
	arr[row][col] = 4;
	delete[] found;
}


CameraPos moving_cursor(CameraPos camera) {
	static int prevMouseX = 0, prevMouseY = 0;

	int mouseX, mouseY;
	AEInputGetCursorPosition(&mouseX, &mouseY);

	if (AEInputCheckCurr(AEVK_MBUTTON)) {
		int deltaX = mouseX - prevMouseX;
		int deltaY = mouseY - prevMouseY;

		float sensitivity = 1.0f;
		camera.x -= (float)deltaX * sensitivity;
		camera.y += (float)deltaY * sensitivity;

	}

	prevMouseX = mouseX;
	prevMouseY = mouseY;

	return camera;
}


void hover(std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& arr, float cameraX, float cameraY, AEGfxVertexList* mesh,int current_tile) {

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetTransparency(0.5f);
	AEMtx33 scale, translate, transform;
	const int EDGE = 4;
	int mouseX, mouseY;
	AEInputGetCursorPosition(&mouseX, &mouseY);
	int gridX = (int)((mouseX + cameraX - 25) / TILE_SIZE);
	int gridY = (int)((mouseY - cameraY - 25) / TILE_SIZE);
	float x = (gridX * TILE_SIZE) - 800.0f + 25.0f;
	float y = 450.0f - (gridY * TILE_SIZE) - 25.0f;
	float screenX = x - cameraX;
	float screenY = y - cameraY;

	float mouseScreenX = mouseX - 800.0f;
	float mouseScreenY = 450.0f - mouseY;
	if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT){
		PrintBlock(3, 1.0f, mouseScreenX, mouseScreenY, meshes.circle);
	}
	else {
		if (arr[gridY][gridX] == 0) {
			PrintBlock(current_tile, 0.3, screenX, screenY, meshes.square);
		}
		else {
			AEGfxSetTransparency(0.5f);
			AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
			AEMtx33Scale(&scale, (float)TILE_SIZE, (float)TILE_SIZE);
			AEMtx33Trans(&translate, screenX, screenY);
			AEMtx33Concat(&transform, &translate, &scale);
			AEGfxSetTransform(transform.m);
			AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
			AEGfxSetTransparency(1.0f);
		}
	}
}
void zoom(int& TILE_SIZE, float& cameraX, float& cameraY) {
	int scrollDelta = 0;
	AEInputMouseWheelDelta(&scrollDelta);

	if (scrollDelta > 0 && TILE_SIZE <= 50) {
		TILE_SIZE += 1;
	}
	else if (scrollDelta < 0 && TILE_SIZE >= 14) {
		TILE_SIZE -= 1;
	}

	float worldSize = 64.0f * TILE_SIZE;
	float screenW = (float)AEGfxGetWindowWidth();
	float screenH = (float)AEGfxGetWindowHeight();

	float maxY = worldSize - screenH - 100.0f;
	if (maxY < 0.0f) maxY = 0.0f;

	cameraX = (worldSize - screenW) / 2.0f;
	cameraY = AEClamp(cameraY, 0.0f, maxY);
}