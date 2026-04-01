/*!************************************************************************
  \file map.cpp
  \author Zhang Yida
  \brief
  This source file implements the map system for a tile-based game, including
  loading and saving level data from files, rendering tiles with a 3D bevel
  effect, spawning flags at randomised spawn points, flood-fill validation
  for detecting enclosed holes, camera movement and clamping, and editor
  utilities such as tile hover preview and scroll-to-zoom.
**************************************************************************/

#include "AEEngine.h"
#include "map.hpp"
#include <fstream>
#include <array>
#include <cstdlib>
#include <random>
#include "utils.hpp"  
#include <stack>
#include <utility>
#include "gameConstants.hpp"
#include <vector>

std::random_device rd;      /*!< Non-deterministic seed source for the RNG. */
std::mt19937 gen(rd());     /*!< Mersenne Twister PRNG seeded from rd. */
float MAP_HEIGHT{};         /*!< Number of tile rows in the loaded map. */
float MAP_WIDTH{};          /*!< Number of tile columns in the loaded map. */
float TILE_SIZE{};          /*!< Current side length of one tile in pixels. */
std::vector<std::vector<int>> newMap; /*!< Padded copy of the map used by flood-fill validation. */

/*!************************************************************************
  \brief
    Opens a map file and reads its header (height, width, tile size) and
    tile data into the provided 2D array. Also allocates the padded newMap
    used later by flood-fill validation.

  \param filename
    Path to the map text file to load.

  \param arr
    Output 2D vector that will be resized and filled with tile IDs.

  \return
    Returns 1 on success, 0 if the file could not be opened or tile data
    could not be read.
**************************************************************************/
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

/*!************************************************************************
  \brief
    Writes the current map dimensions, the default tile size, and every
    tile ID to a text file so the level can be reloaded later.

  \param filename
    Path of the output file to create or overwrite.

  \param arr
    2D vector of tile IDs representing the current map layout.
**************************************************************************/
void Save_Game_Level(const char* filename, std::vector<std::vector<int>>& arr) {
    std::ofstream map_file(filename);
    if (!map_file.is_open()) {
        return;
    }

    map_file << "MAP_HEIGHT = " << MAP_HEIGHT << std::endl;
    map_file << "MAP_WIDTH = " << MAP_WIDTH << std::endl;
    map_file << "TILE_SIZE = " << MAP_DEFAULT_TILE_SIZE << std::endl;

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map_file << arr[i][j];
            if (j < MAP_WIDTH - 1) map_file << " ";
        }
        map_file << std::endl;
    }

    map_file.close();
}

/*!************************************************************************
  \brief
    Iterates over every tile in the map and calls PrintBlock to render it
    at the correct world-to-screen position. Spawn marker tiles (player,
    enemy, flag spawn) are skipped when not in editor mode so they are
    invisible during gameplay.

  \param arr
    2D vector of tile IDs to render.

  \param camera
    Current camera position used to offset world coordinates to screen
    coordinates.

  \param editorMode
    When true, spawn marker tiles are rendered; when false they are hidden.
**************************************************************************/
void Load_Map(std::vector<std::vector<int>>& arr, CameraPos& camera, bool editorMode) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (arr[i][j] == EMPTY) continue;
            if (!editorMode && (arr[i][j] == PLAYER_SPAWN ||
                arr[i][j] == ENEMY_SPAWN ||
                arr[i][j] == FLAG_SPAWN)) continue;

            float x = (j * TILE_SIZE) - (SCREEN_WIDTH * 0.5f) + MAP_TILE_OFFSET;
            float y = (SCREEN_HEIGHT * 0.5f) - (i * TILE_SIZE) - MAP_TILE_OFFSET;
            float screenX = x - camera.x;
            float screenY = y - camera.y;
            PrintBlock(arr[i][j], 1.0f, screenX, screenY, meshes.square);
        }
    }
}

/*!************************************************************************
  \brief
    Draws a single tile with a three-pass 3D bevel effect. The first pass
    renders a dark shadow offset to the bottom-right, the second renders a
    bright face offset to the top-left, and the third renders the slightly
    darkened main face on top, creating a raised-block appearance.

  \param choice
    Tile ID used to determine the base RGB colour of the block.

  \param d
    Alpha/transparency multiplier applied to every draw pass.

  \param screenX
    Horizontal screen-space centre of the tile in pixels.

  \param screenY
    Vertical screen-space centre of the tile in pixels.

  \param mesh
    Pointer to the vertex list (mesh) used to draw the tile quad.
**************************************************************************/
void PrintBlock(int choice, float d, float screenX, float screenY, AEGfxVertexList* mesh) {
    /* Determine the base colour from the tile ID. */
    float r = 0.0f, g = 0.0f, b = 0.0f;
    if (choice == WALL) { r = 0.0f; g = 1.0f; b = 0.0f; }
    else if (choice == GOAL) { r = 0.5f; g = 0.0f; b = 1.0f; }
    else if (choice == ENEMY_SPAWN) { r = 1.0f; g = 0.0f; b = 0.0f; }
    else if (choice == FLAG) { r = 0.0f; g = 1.0f; b = 1.0f; }
    else if (choice == FLAG_SPAWN) { r = 0.0f; g = 1.0f; b = 1.0f; }
    else if (choice == PLAYER_SPAWN) { r = 1.0f; g = 1.0f; b = 0.0f; }

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetTransparency(1.0f);
    AEMtx33 scale, translate, transform;
    const int EDGE = 4; /*!< Pixel offset used to produce the bevel depth. */

    /* Pass 1 – dark shadow layer, shifted down-right to simulate depth. */
    AEGfxSetColorToMultiply(r * 0.2f, g * 0.2f, b * 0.2f, d);
    AEMtx33Scale(&scale, (float)(TILE_SIZE + EDGE), (float)(TILE_SIZE + EDGE));
    AEMtx33Trans(&translate, screenX + EDGE * 0.5f, screenY - EDGE * 0.5f);
    AEMtx33Concat(&transform, &translate, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

    /* Pass 2 – bright highlight layer, shifted up-left to simulate a lit edge. */
    AEGfxSetColorToMultiply(r, g, b, d);
    AEMtx33Scale(&scale, (float)(TILE_SIZE + EDGE), (float)(TILE_SIZE + EDGE));
    AEMtx33Trans(&translate, screenX - EDGE * 0.5f, screenY + EDGE * 0.5f);
    AEMtx33Concat(&transform, &translate, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

    /* Pass 3 – main face, slightly darkened and drawn at the true tile size. */
    AEGfxSetColorToMultiply(r * 0.35f, g * 0.35f, b * 0.35f, d);
    AEMtx33Scale(&scale, (float)TILE_SIZE, (float)TILE_SIZE);
    AEMtx33Trans(&translate, screenX, screenY);
    AEMtx33Concat(&transform, &translate, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

/*!************************************************************************
  \brief
    Collects every FLAG_SPAWN tile in the map, then randomly selects one
    and converts it to a FLAG tile, placing the flag for the current round.
    If no spawn tiles exist the function exits silently.

  \param arr
    2D vector of tile IDs. The selected FLAG_SPAWN cell is overwritten with
    FLAG in place.
**************************************************************************/
void Spawn_Flag(std::vector<std::vector<int>>& arr) {
    /* Collect the grid coordinates of all flag-spawn tiles. */
    std::vector<std::pair<int, int>> flagSpawns;

    for (int i = 0; i < MAP_HEIGHT; i++)
        for (int j = 0; j < MAP_WIDTH; j++)
            if (arr[i][j] == FLAG_SPAWN)
                flagSpawns.push_back({ i, j });

    if (flagSpawns.empty()) return;

    /* Pick one spawn at random and place the flag there. */
    std::uniform_int_distribution<int> dist(0, (int)flagSpawns.size() - 1);
    int index = dist(gen);
    arr[flagSpawns[index].first][flagSpawns[index].second] = FLAG;
}

/*!************************************************************************
  \brief
    Pans the camera by tracking middle-mouse-button drag deltas each frame.
    After panning, the camera position is clamped to the map boundaries plus
    a small configurable padding so the view cannot scroll outside the level.

  \param camera
    Current camera position. The updated position is returned by value.

  \return
    The new camera position after applying drag and boundary clamping.
**************************************************************************/
CameraPos moving_cursor(CameraPos camera) {
    static int prevMouseX = 0, prevMouseY = 0;

    int mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    /* Pan the camera by the mouse delta while the middle button is held. */
    if (AEInputCheckCurr(AEVK_MBUTTON)) {
        int deltaX = mouseX - prevMouseX;
        int deltaY = mouseY - prevMouseY;

        float sensitivity = 1.0f;
        camera.x -= (float)deltaX * sensitivity;
        camera.y += (float)deltaY * sensitivity;
    }

    prevMouseX = mouseX;
    prevMouseY = mouseY;

    /* Compute maximum scroll extents from the world and screen sizes. */
    float worldSize = MAP_WORLD_SIZE_TILES * TILE_SIZE;
    float screenW = (float)AEGfxGetWindowWidth();
    float screenH = (float)AEGfxGetWindowHeight();

    float maxX = worldSize - screenW;
    float minY = -(worldSize - screenH);
    if (maxX < 0.0f) maxX = 0.0f;
    if (minY > 0.0f) minY = 0.0f;

    /* Apply padding and clamp to keep the camera within valid bounds. */
    float paddingX = MAP_CAMERA_PADDING_X / (float)TILE_SIZE * 10.0f;
    float paddingY = MAP_CAMERA_PADDING_Y / (float)TILE_SIZE * 2.0f;

    if (camera.x < -paddingX)              camera.x = -paddingX;
    if (camera.x > maxX + MAP_TILE_OFFSET) camera.x = maxX + MAP_TILE_OFFSET;
    if (camera.y < minY - paddingY)        camera.y = minY - paddingY;
    if (camera.y > paddingY)               camera.y = paddingY;

    return camera;
}

/*!************************************************************************
  \brief
    Renders a semi-transparent preview of the currently selected tile under
    the mouse cursor while in editor mode. If the cursor is over an occupied
    tile a dark overlay is drawn instead to indicate the cell cannot be
    painted. If the cursor is outside the map grid a circle marker is shown.

  \param arr
    2D vector of tile IDs used to check whether the hovered cell is empty.

  \param cameraX
    Horizontal camera offset in pixels.

  \param cameraY
    Vertical camera offset in pixels.

  \param mesh
    Mesh used to draw the dark occupied-cell overlay.

  \param current_tile
    Tile ID of the tile type currently selected by the editor.
**************************************************************************/
void hover(std::vector<std::vector<int>>& arr, float cameraX, float cameraY,
    AEGfxVertexList* mesh, int current_tile) {
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetTransparency(0.5f);
    AEMtx33 scale, translate, transform;

    int mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    /* Convert screen-space mouse position to map grid indices. */
    int gridX = (int)((mouseX + cameraX - MAP_TILE_OFFSET) / TILE_SIZE);
    int gridY = (int)((mouseY - cameraY - MAP_TILE_OFFSET) / TILE_SIZE);

    /* World-space position of the hovered tile's centre. */
    float x = (gridX * TILE_SIZE) - (SCREEN_WIDTH * 0.5f) + MAP_TILE_OFFSET;
    float y = (SCREEN_HEIGHT * 0.5f) - (gridY * TILE_SIZE) - MAP_TILE_OFFSET;
    float screenX = x - cameraX;
    float screenY = y - cameraY;

    /* Raw screen-space mouse position (for the out-of-bounds fallback). */
    float mouseScreenX = mouseX - (SCREEN_WIDTH * 0.5f);
    float mouseScreenY = (SCREEN_HEIGHT * 0.5f) - mouseY;

    if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) {
        /* Cursor is outside the map – draw a circle at the raw mouse position. */
        PrintBlock(3, 1.0f, mouseScreenX, mouseScreenY, meshes.circle);
    }
    else {
        if (arr[gridY][gridX] == 0) {
            /* Empty cell – preview the selected tile at low opacity. */
            PrintBlock(current_tile, 0.3f, screenX, screenY, meshes.square);
        }
        else {
            /* Occupied cell – draw a dark overlay to show painting is blocked. */
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

/*!************************************************************************
  \brief
    Reads the mouse scroll wheel delta each frame and adjusts the global
    TILE_SIZE accordingly, clamped between MAP_ZOOM_MIN and MAP_ZOOM_MAX.
    Scrolling up zooms in (increases tile size) and scrolling down zooms out.
**************************************************************************/
void zoom() {
    int scrollDelta = 0;
    AEInputMouseWheelDelta(&scrollDelta);

    if (scrollDelta > 0 && TILE_SIZE <= MAP_ZOOM_MAX) TILE_SIZE += 1;
    else if (scrollDelta < 0 && TILE_SIZE >= MAP_ZOOM_MIN) TILE_SIZE -= 1;
}

/*!************************************************************************
  \brief
    Validates the map layout by performing an iterative flood-fill from the
    top-left corner of a border-padded copy of the map. Any empty cell that
    remains unreachable after the fill is an enclosed hole, which means the
    map has an isolated region that enemies or the player cannot access.

  \param arr
    2D vector of tile IDs to validate. Only read; not modified.

  \param x
    Starting row index within the padded newMap (typically 0 for the border).

  \param y
    Starting column index within the padded newMap (typically 0 for the border).
**************************************************************************/
void floodfill(std::vector<std::vector<int>>& arr, int x, int y) {
    std::stack<std::pair<int, int>> stack;

    /* Reset the padded map and copy the real map into its interior. */
    for (int i = 0; i < MAP_HEIGHT + 2; i++)
        for (int j = 0; j < MAP_WIDTH + 2; j++)
            newMap[i][j] = 0;

    for (int i = 0; i < MAP_HEIGHT; i++)
        for (int j = 0; j < MAP_WIDTH; j++)
            newMap[(size_t)i + 1][(size_t)j + 1] = arr[i][j];

    stack.push({ x, y });

    /* Iterative 4-directional flood fill; marks reachable empty cells as -1. */
    while (!stack.empty()) {
        std::pair<int, int> cell = stack.top();
        stack.pop();
        int cx = cell.first;
        int cy = cell.second;

        if (cx < 0 || cy >= MAP_WIDTH + 2 || cy < 0 || cx >= MAP_HEIGHT + 2)
            continue;
        else if (newMap[cx][cy] == 0) {
            /* Reachable empty cell – mark visited and push neighbours. */
            newMap[cx][cy] = -1;
            stack.push({ cx, cy + 1 });
            stack.push({ cx, cy - 1 });
            stack.push({ cx + 1, cy });
            stack.push({ cx - 1, cy });
        }
        else if (newMap[cx][cy] == WALL || newMap[cx][cy] == VISITED ||
            newMap[cx][cy] == GOAL || newMap[cx][cy] == ENEMY_SPAWN ||
            newMap[cx][cy] == FLAG || newMap[cx][cy] == FLAG_SPAWN ||
            newMap[cx][cy] == PLAYER_SPAWN)
            continue; /* Solid or already-visited tile – stop propagating. */
    }
}

/*!************************************************************************
  \brief
    Inspects newMap after a flood-fill to determine whether any empty cell
    (value 0) remains unreached. An unreached empty cell means the map
    contains an enclosed hole that the flood-fill could not enter, indicating
    an invalid map layout.

  \return
    Returns true if every empty cell was reached (no holes found),
    false if at least one unreachable empty cell exists.
**************************************************************************/
bool HasHole() {
    bool hole = true;
    for (int i = 0; i < MAP_HEIGHT + 2; i++)
        for (int j = 0; j < MAP_WIDTH + 2; j++)
            if (newMap[i][j] == 0) { hole = false; }
    return hole;
}

/*!************************************************************************
  \brief
    Smoothly moves the camera toward a target world position using linear
    interpolation, then clamps the result to the map boundaries so the
    camera can never scroll beyond the level edges.

  \param camera
    Camera state to update in place. The lerpSpeed field controls how
    quickly the camera closes the gap each frame.

  \param targetX
    Desired horizontal world-space camera position to lerp toward.

  \param targetY
    Desired vertical world-space camera position to lerp toward.

  \param dt
    Delta time in seconds for the current frame, used to keep the lerp
    frame-rate independent.
**************************************************************************/
void Update_CameraPosition(CameraPos& camera, float targetX, float targetY, float dt) {
    /* Lerp the camera toward the target position. */
    camera.x += (targetX - camera.x) * camera.lerpSpeed * dt;
    camera.y += (targetY - camera.y) * camera.lerpSpeed * dt;

    float halfW = SCREEN_WIDTH * 0.5f;
    float halfH = SCREEN_HEIGHT * 0.5f;
    float mapW = (float)MAP_WIDTH * (float)TILE_SIZE;
    float mapH = (float)MAP_HEIGHT * (float)TILE_SIZE;

    /* Compute the scroll limits from the map and screen dimensions. */
    float minX = 0.0f;
    float maxX = mapW - halfW - halfW;
    float minY = -(mapH - halfH) + halfH;
    float maxY = 0.0f;

    /* Clamp with a small buffer to avoid a hard edge at the boundary. */
    camera.x = AEClamp(camera.x, minX - MAP_CAMERA_CLAMP_BUFFER, maxX + MAP_CAMERA_CLAMP_BUFFER);
    camera.y = AEClamp(camera.y, minY - MAP_CAMERA_CLAMP_BUFFER, maxY + MAP_CAMERA_CLAMP_BUFFER);
}

/*!************************************************************************
  \brief
    Searches the map for a PLAYER_SPAWN tile and returns its world-space
    position. If no spawn tile is found both output values are set to 0.

  \param arr
    2D vector of tile IDs to search.

  \param outX
    Output parameter set to the world-space X coordinate of the spawn tile.

  \param outY
    Output parameter set to the world-space Y coordinate of the spawn tile.
**************************************************************************/
void FindPlayerSpawn(std::vector<std::vector<int>>& arr, float& outX, float& outY) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (arr[i][j] == PLAYER_SPAWN) {
                outX = (j * TILE_SIZE) - (SCREEN_WIDTH * 0.5f) + MAP_TILE_OFFSET;
                outY = (SCREEN_HEIGHT * 0.5f) - (i * TILE_SIZE) - MAP_TILE_OFFSET;
                return;
            }
        }
    }

    /* No spawn tile found – default to the world origin. */
    outX = 0.0f;
    outY = 0.0f;
}