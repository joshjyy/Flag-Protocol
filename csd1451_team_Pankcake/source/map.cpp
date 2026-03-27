/*!************************************************************************
  \file map.cpp
  \author






  \brief
  This source file contains definitions for the map layout.

**************************************************************************/

#include "AEEngine.h"
#include <iostream>
#include "map.hpp"
#include <fstream>
#include <array>
#include <cstdlib>
#include <random>
#include "utils.hpp"  
#include <stack>
#include <utility>
#include "gameConstants.hpp"
#include "popup.hpp"
#include <vector>

std::random_device rd;
std::mt19937 gen(rd());	
float MAP_HEIGHT{};
float MAP_WIDTH{};
float TILE_SIZE{};
std::vector<std::vector<int>> newMap;
Popup popup;
/* 
 * Loads a game level map from a text file into a 2D array.
 * Opens the specified file and reads MAP_HEIGHT x MAP_WIDTH integers.
 * Each integer represents a tile type (0 = empty, 1 = solid).
 * Returns 1 on success, 0 on failure (file not found or invalid format).
 * Prints error messages if file cannot be opened or data is incomplete.
 */
int Load_Game_Level(const char* filename, std::vector<std::vector<int>>& arr) {
	std::ifstream map_file(filename);
	if (!map_file.is_open()) return 0;

	std::string token;
	map_file >> token >> token >> MAP_HEIGHT;
	map_file >> token >> token >> MAP_WIDTH;
	map_file >> token >> token >> TILE_SIZE;


	arr.assign((size_t)MAP_HEIGHT, std::vector<int>((size_t)MAP_WIDTH, 0));       
	newMap.assign((size_t)(MAP_HEIGHT + 2), std::vector<int>((size_t)(MAP_WIDTH + 2), 0)); 

	for (int i = 0; i < MAP_HEIGHT; i++)
		for (int j = 0; j < MAP_WIDTH; j++)
			if (!(map_file >> arr[i][j])) return 0;

	return 1;
}

/**
 * Saves a game level map from a 2D array into a text file.
 * Opens the specified file and writes MAP_HEIGHT x MAP_WIDTH integers.
 * Each integer represents a tile type (0 = empty, 1-5 = different tiles).
 **/
void Save_Game_Level(const char* filename, std::vector<std::vector<int>>& arr) {
	std::ofstream map_file(filename);
	if (!map_file.is_open()) {
		std::cout << "Error: Could not save file " << filename << std::endl;
		return;
	}

	// Add the headers that Load_Game_Level expects
	map_file << "MAP_HEIGHT = " << MAP_HEIGHT << std::endl;
	map_file << "MAP_WIDTH = " << MAP_WIDTH << std::endl;
	map_file << "TILE_SIZE = " << 50 << std::endl;

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map_file << arr[i][j];
			if (j < MAP_WIDTH - 1) map_file << " ";
		}
		map_file << std::endl;
	}

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
  * 1 = red (walls/obstacl	es)
  * 2 = blue (water)
  * 3 = green (grass)
  * 4 = yellow (sand)
  * 5 = purple (special tiles)
  */
void Load_Map(std::vector<std::vector<int>>& arr, CameraPos& camera, bool editorMode) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (arr[i][j] == EMPTY || arr[i][j] == FLAG_SPAWN) continue;
			if (!editorMode && (arr[i][j] == PLAYER_SPAWN || arr[i][j] == ENEMY_SPAWN)) continue;

			float x = (j * TILE_SIZE) - 800.0f + 25.0f;
			float y = 450.0f - (i * TILE_SIZE) - 25.0f;
			float screenX = x - camera.x;
			float screenY = y - camera.y;
			PrintBlock(arr[i][j], 1.0f, screenX, screenY, meshes.square);
		}
	}
}

void PrintBlock(int choice, float d, float screenX, float screenY, AEGfxVertexList* mesh) {
	float r = 0.0f, g = 0.0f, b = 0.0f;
	if (choice == WALL) { r = 0.0f; g = 1.0f; b = 0.0f; }  // green blocks
	else if (choice == GOAL) { r = 0.5f; g = 0.0f; b = 1.0f; }  // purple goal
	else if (choice == ENEMY_SPAWN) { r = 1.0f; g = 0.0f; b = 0.0f; }  // red
	else if (choice == FLAG) { r = 0.0f; g = 1.0f; b = 1.0f; }  // lime flag
	else if (choice == PLAYER_SPAWN) { r = 1.0f; g = 1.0f; b = 0.0f; } 
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

void Spawn_Flag(std::vector<std::vector<int>>& arr) {
	struct Tile { int row, col; };

	int count = 0;
	for (int i = 0; i < MAP_HEIGHT; i++)
		for (int j = 0; j < MAP_WIDTH; j++)
			if (arr[i][j] == FLAG_SPAWN) count++;

	if (count == 0) { std::cout << "No flag tiles found!\n"; return; }

	Tile* found = new Tile[count];
	int flagloc = 0;
	for (int i = 0; i < MAP_HEIGHT; i++)
		for (int j = 0; j < MAP_WIDTH; j++)
			if (arr[i][j] == FLAG_SPAWN) { found[flagloc].row = i; found[flagloc].col = j; flagloc++; }

	srand((unsigned int)time(0));
	int index = rand() % count;
	arr[found[index].row][found[index].col] = FLAG;
	std::cout << "Flag spawned at Row: " << found[index].row << " Col: " << found[index].col << "\n";
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

	float worldSize = 64.0f * TILE_SIZE;
	float screenW = (float)AEGfxGetWindowWidth();
	float screenH = (float)AEGfxGetWindowHeight();

	float maxX = worldSize - screenW;
	float minY = -(worldSize - screenH);
	if (maxX < 0.0f) maxX = 0.0f;
	if (minY > 0.0f) minY = 0.0f;
	float paddingX = 500.0f / (float)TILE_SIZE * 10.0f;
	float paddingY = 500.0f / (float)TILE_SIZE * 2.0f;

	if (camera.x < -paddingX)        camera.x = -paddingX;
	if (camera.x > maxX + 50.0f)    camera.x = maxX + 50.0f;
	if (camera.y < minY - paddingY)  camera.y = minY - paddingY;
	if (camera.y > paddingY)         camera.y = paddingY;
	return camera;
}

void hover(std::vector<std::vector<int>>& arr, float cameraX, float cameraY, AEGfxVertexList* mesh,int current_tile) {

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetTransparency(0.5f);
	AEMtx33 scale, translate, transform;
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
			PrintBlock(current_tile, 0.3f, screenX, screenY, meshes.square);
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
void zoom() {
	int scrollDelta = 0;
	AEInputMouseWheelDelta(&scrollDelta);

	if (scrollDelta > 0 && TILE_SIZE <= 50) {
		TILE_SIZE += 1;
	}
	else if (scrollDelta < 0 && TILE_SIZE >= 14) {
		TILE_SIZE -= 1;
	}

}

void floodfill(std::vector<std::vector<int>>& arr, int x, int y)
{
	std::stack<std::pair<int, int>> stack;
	for (int i = 0; i < MAP_HEIGHT + 2; i++)
		for (int j = 0; j < MAP_WIDTH + 2; j++)
			newMap[i][j] = 0;
	
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			newMap[i + 1][j + 1] =arr[i][j];
		}
	}
	stack.push({ x, y });

	while (!stack.empty())
	{
		std::pair<int, int> cell = stack.top();
		stack.pop();
		int cx = cell.first;
		int cy = cell.second;

		if (cx < 0 || cy >= MAP_WIDTH+2 || cy < 0 || cx >= MAP_HEIGHT+2) {
			continue;
		}
		else if (newMap[cx][cy] == 0) {
			newMap[cx][cy] = -1;
			stack.push({ cx, cy + 1 });
			stack.push({ cx, cy - 1});
			stack.push({ cx + 1, cy });
			stack.push({ cx - 1, cy });
		}
		else if (newMap[cx][cy] == WALL || newMap[cx][cy] == VISITED ||
			newMap[cx][cy] == GOAL || newMap[cx][cy] == ENEMY_SPAWN ||
			newMap[cx][cy] == FLAG || newMap[cx][cy] == FLAG_SPAWN ||
			newMap[cx][cy] == PLAYER_SPAWN) {
			continue;
		}
		

	}
}

bool HasHole() {
	bool hole = true;
	for (int i = 0; i < MAP_HEIGHT + 2; i++) {
		for (int j = 0; j < MAP_WIDTH + 2; j++) {
			if (newMap[i][j] == 0) {
				hole = false;
			}
		}
	}

	if (hole) {
		std::cout << "leak exists" << std::endl;
		Popup_Show(popup, "WARNING: LEAK FOUND", 3.0f, 1.0f, 0.0f, 0.0f);	
	}
	else {
		std::cout << "no leak" << std::endl;
		Popup_Show(popup, "Map Saved Successfully", 3.0f, 0.0f, 1.0f, 0.0f);
	}

	return hole;
}

void Update_CameraPosition(CameraPos& camera, float targetX, float targetY, float dt) {
	camera.x += (targetX - camera.x) * camera.lerpSpeed * dt;
	camera.y += (targetY - camera.y) * camera.lerpSpeed * dt;

	float halfW = 800.0f;
	float halfH = 450.0f;

	float mapW = (float)MAP_WIDTH * (float)TILE_SIZE; 
	float mapH = (float)MAP_HEIGHT * (float)TILE_SIZE; 

	float minX = -halfW + halfW;          
	float maxX = mapW - halfW - halfW;   
	float minY = -(mapH - halfH) + halfH; 
	float maxY = 0.0f;

	camera.x = AEClamp(camera.x, minX - 200.0f, maxX + 200.0f);
	camera.y = AEClamp(camera.y, minY - 200.0f, maxY + 200.0f);
}

void FindPlayerSpawn(std::vector<std::vector<int>>& arr, float& outX, float& outY) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (arr[i][j] == PLAYER_SPAWN) {
				outX = (j * TILE_SIZE) - 800.0f + 25.0f;
				outY = 450.0f - (i * TILE_SIZE) - 25.0f;
				return;
			}
		}
	}
	
	outX = 0.0f;
	outY = 0.0f;
}