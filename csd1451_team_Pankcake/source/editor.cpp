/*!************************************************************************
  \file editor.cpp
  \author






  \brief
  This source file contains definitions for the editor game mode.

**************************************************************************/

#include "editor.hpp"
#include "utils.hpp"
#include <cstdio>
#include "sound.hpp"

Popup popup;

std::vector<std::vector<int>> g_initialEditorMap;

EditorState InitEditor(void) {
    return EditorState();
}

void UpdateAndRenderEditor(EditorState& editor,
    std::vector<std::vector<int>>& gameMap,
    AEGfxVertexList* squareMesh
    ) {

    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);

    // Handle all input
    HandleEditorInput(editor, gameMap);

    // Camera movement
    editor.camera = moving_cursor(editor.camera);

    // Load and render the map
    Load_Map(gameMap, editor.camera, true);

    // Draw hover preview
    DrawEditorHover(editor, gameMap, squareMesh);

    // Render UI
    RenderEditorUI(editor, squareMesh);
}

// Saves custom map
void SaveCustomMap(std::vector<std::vector<int>>& gameMap, EditorState& editor) {
    floodfill(gameMap, 0, 0);

    if (HasHole()) {
        Popup_Show(popup, "Map has holes! Fix before saving.", 3.0f, 1.0f, 0.0f, 0.0f);
        return;
    }

    char errorMsg[128] = "";

    if (editor.flag_count < 5)
        strcat_s(errorMsg, "Need 5 flags!\n ");

    if (editor.enemy_spawner_count < 2)
        strcat_s(errorMsg, "Need 2 spawners!\n ");

    if (editor.player_spawn_count < 1)
        strcat_s(errorMsg, "Need 1 player spawn!\n");

    if (editor.goal_count < 1)
        strcat_s(errorMsg, "Need 1 Goal!\n");

    if (strlen(errorMsg) > 0) {
        Popup_Show(popup, errorMsg, 3.0f, 1.0f, 0.0f, 0.0f);
        return;
    }

    Save_Game_Level("Assets/My_Custom_Map.txt", gameMap);
    Popup_Show(popup, "Map Saved Successfully.", 3.0f, 0.0f, 1.0f, 0.0f);
}
// Loads custom map
void LoadCustomMap(std::vector<std::vector<int>>& gameMap) {
    if (Load_Game_Level("Assets/My_Custom_Map.txt", gameMap)) {
        //printf("Custom map loaded from Assets/My_Custom_Map.txt!\n");
    }
    else {
        //printf("No custom map found! Save one first with 'S'.\n");
    }
}

// Loads initial map layout
void ResetToInitialEditorMap(std::vector<std::vector<int>>& gameMap) {
    gameMap = g_initialEditorMap;
    //printf("Reset to initial editor map!\n");

}

// Save the initial editor map (called when editor first loads)
void SaveInitialEditorMap(std::vector<std::vector<int>>& gameMap) {
    g_initialEditorMap = gameMap;
    //printf("Initial editor state saved!\n");
}

void ClearTileType(std::vector<std::vector<int>>& gameMap, int tileType) {
    for (int i = 0; i < MAP_HEIGHT; i++)
        for (int j = 0; j < MAP_WIDTH; j++)
            if (gameMap[i][j] == tileType)
                gameMap[i][j] = EMPTY;
}

// Input handling
void HandleEditorInput(EditorState& editor,
    std::vector<std::vector<int>>& gameMap) {

    editor.flag_count = 0;
    editor.enemy_spawner_count = 0;
    editor.player_spawn_count = 0;
    editor.goal_count = 0;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (gameMap[y][x] == FLAG) editor.flag_count++; 
            if (gameMap[y][x] == ENEMY_SPAWN) editor.enemy_spawner_count++;
            if (gameMap[y][x] == PLAYER_SPAWN) editor.player_spawn_count++;
            if (gameMap[y][x] == GOAL) editor.goal_count++;
        }
    }

    // Get mouse position
    AEInputGetCursorPosition(&editor.mouseX, &editor.mouseY);

    // Convert to grid coordinates
    int gridX = (int)((editor.mouseX + editor.camera.x - MAP_TILE_OFFSET) / TILE_SIZE);
    int gridY = (int)((editor.mouseY - editor.camera.y - MAP_TILE_OFFSET) / TILE_SIZE);

    // Initialize paint readiness
    if (!editor.editorPaintReady) {
        if (!AEInputCheckCurr(AEVK_LBUTTON)) {
            editor.editorPaintReady = true;
        }
    }

    // Check if within grid bounds
    if (gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT) {
        if (editor.editorPaintReady && AEInputCheckCurr(AEVK_LBUTTON)) {
                if (editor.currentTileType == PLAYER_SPAWN)
                    ClearTileType(gameMap, PLAYER_SPAWN);

                if (editor.currentTileType == PLAYER_SPAWN)
                    ClearTileType(gameMap, PLAYER_SPAWN);

                if (editor.currentTileType == GOAL)
                    ClearTileType(gameMap, GOAL);

                if (editor.currentTileType == FLAG) {
                    if (editor.flag_count < 10)
                        gameMap[gridY][gridX] = editor.currentTileType;
                }
                else if (editor.currentTileType == ENEMY_SPAWN) {
                    if (editor.enemy_spawner_count < 10)
                        gameMap[gridY][gridX] = editor.currentTileType;
                }
                else {
                    gameMap[gridY][gridX] = editor.currentTileType;
                }

            //printf("flag_count: %d\n", editor.flag_count);
            //printf("enemy_spawner_count: %d\n", editor.enemy_spawner_count);
            //printf("player_spawn_count: %d\n", editor.player_spawn_count);
        }

        if (AEInputCheckCurr(AEVK_RBUTTON)) {
            gameMap[gridY][gridX] = 0;
            //printf("flag_count: %d\n", editor.flag_count);
            //printf("enemy_spawner_count: %d\n", editor.enemy_spawner_count);
            //printf("player_spawn_count: %d\n", editor.player_spawn_count);

        }
    }

    // Number keys to change tile type
    if (AEInputCheckTriggered(AEVK_0)) editor.currentTileType = 0;  // Eraser
    if (AEInputCheckTriggered(AEVK_1)) editor.currentTileType = 1;  // Green blocks
    if (AEInputCheckTriggered(AEVK_2)) editor.currentTileType = 2;  // Purple goal
    if (AEInputCheckTriggered(AEVK_3)) editor.currentTileType = 3;  // Red blocks
    if (AEInputCheckTriggered(AEVK_4)) editor.currentTileType = 4;  // Lime flag
    if (AEInputCheckTriggered(AEVK_5)) editor.currentTileType = PLAYER_SPAWN;


    // Save custom map (S key)
    if (AEInputCheckTriggered(AEVK_S)) {
        printf("flags: %d spawners: %d\n", editor.flag_count, editor.enemy_spawner_count);
        SaveCustomMap(gameMap, editor);
    }

    // Load custom map (L key)
    if (AEInputCheckTriggered(AEVK_L)) {
        LoadCustomMap(gameMap);
    }

    // Reload initial map (I key)
    if (AEInputCheckTriggered(AEVK_I)) {
        ResetToInitialEditorMap(gameMap);
    }

    // Clear map (C key)
    if (AEInputCheckTriggered(AEVK_C)) {
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                gameMap[i][j] = 0;
            }
        }
        //printf("Map cleared!\n");
    }

    // Zoom
    zoom();
}

void DrawEditorHover(const EditorState& editor,
    std::vector<std::vector<int>>& gameMap,
    AEGfxVertexList* squareMesh) {

    int gridX = (int)((editor.mouseX + editor.camera.x - MAP_TILE_OFFSET) / TILE_SIZE);
    int gridY = (int)((editor.mouseY - editor.camera.y - MAP_TILE_OFFSET) / TILE_SIZE);

    float x = (gridX * TILE_SIZE) - (SCREEN_WIDTH * 0.5f) + MAP_TILE_OFFSET;
    float y = (SCREEN_HEIGHT * 0.5f) - (gridY * TILE_SIZE) - MAP_TILE_OFFSET;

    float screenX = x - editor.camera.x;
    float screenY = y - editor.camera.y;

    float mouseScreenX = editor.mouseX - (SCREEN_WIDTH * 0.5f);
    float mouseScreenY = (SCREEN_HEIGHT * 0.5f) - editor.mouseY;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetTransparency(0.5f);

    if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) {
        PrintBlock(3, 1.0f, mouseScreenX, mouseScreenY, meshes.circle);
    }
    else {
        if (gameMap[gridY][gridX] == 0) {
            PrintBlock(editor.currentTileType, 0.3f, screenX, screenY, squareMesh);
        }
        else {
            AEMtx33 scale, translate, transform;
            AEGfxSetTransparency(0.5f);
            AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
            AEMtx33Scale(&scale, (float)TILE_SIZE, (float)TILE_SIZE);
            AEMtx33Trans(&translate, screenX, screenY);
            AEMtx33Concat(&transform, &translate, &scale);
            AEGfxSetTransform(transform.m);
            AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);
            AEGfxSetTransparency(1.0f);
        }
    }
}

void RenderEditorUI(const EditorState& editor,
    AEGfxVertexList* squareMesh) {
    (void)squareMesh;

    float uiY = 350.0f;
    float textX, textY;

    // Title
    GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
    AEGfxPrint(fontID, "EDITOR MODE", textX, textY, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    // Tiles section
    uiY -= 40.0f;
    GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
    AEGfxPrint(fontID, "TILES", textX, textY, 1.0f, 0.8f, 0.8f, 0.8f, 1.0f);

    uiY -= 30.0f;

    // Tile options in columns
    struct TileDisplay {
        const char* label;
        float x;
        float r, g, b;
    };

    TileDisplay tiles[] = {
        {"1: Green Blocks", -700.0f, 0.0f, 1.0f, 0.0f},
        {"2: Purple Goal", -550.0f, 0.5f, 0.0f, 1.0f},
        {"3: Red Blocks", -400.0f, 1.0f, 0.0f, 0.0f},
        {"4: Lime Flag", -700.0f, 0.0f, 1.0f, 1.0f},
        {"5: Player Spawn", -550.0f, 1.0f, 1.0f, 0.0f},
        {"0/RMB: Eraser", -400.0f, 1.0f, 1.0f, 1.0f}
    };

    // First row (1-3)
    for (int i = 0; i < 3; i++) {
        GetNormalizedTextPosition(tiles[i].x, uiY, &textX, &textY);
        AEGfxPrint(fontID, tiles[i].label, textX, textY, 1.0f,
            tiles[i].r, tiles[i].g, tiles[i].b, 1.0f);
    }

    uiY -= 30.0f;

    // Second row (4 and Eraser)
    for (int i = 3; i < 6; i++) {
        GetNormalizedTextPosition(tiles[i].x, uiY, &textX, &textY);
        AEGfxPrint(fontID, tiles[i].label, textX, textY, 1.0f,
            tiles[i].r, tiles[i].g, tiles[i].b, 1.0f);
    }

    uiY -= 50.0f;

    // Current tile selection
    GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
    AEGfxPrint(fontID, "CURRENT", textX, textY, 1.0f, 0.8f, 0.8f, 0.8f, 1.0f);

    uiY -= 30.0f;

    // Show current tile with color
    GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
    char tileText[32];

    const char* tileNames[] = { "Eraser", "Wall", "Goal", "Enemy Spawn", "Flag", "Flag Spawn", "Player Spawn" };
    float tileColors[7][3] = {
        {1.0f, 1.0f, 1.0f},  // eraser
        {0.0f, 1.0f, 0.0f},  // wall green
        {0.5f, 0.0f, 1.0f},  // goal purple
        {1.0f, 0.0f, 0.0f},  // enemy spawn red
        {0.0f, 1.0f, 1.0f},  // flag cyan
        {0.5f, 0.5f, 1.0f},  // flag spawn blue
        {1.0f, 1.0f, 0.0f},  // player spawn yellow
    };

    sprintf_s(tileText, 32, "Current: %s", tileNames[editor.currentTileType]);

    AEGfxPrint(fontID, tileText, textX, textY, 1.0f,
        tileColors[editor.currentTileType][0],
        tileColors[editor.currentTileType][1],
        tileColors[editor.currentTileType][2], 1.0f);

    // Commands
    uiY -= 40.0f;
    GetNormalizedTextPosition(-700.0f, uiY, &textX, &textY);
    AEGfxPrint(fontID, "COMMANDS", textX, textY, 1.0f, 0.8f, 0.8f, 0.8f, 1.0f);

    uiY -= 30.0f;

    struct CommandDisplay {
        const char* label;
        const char* desc;
        float x;
    };

    CommandDisplay commands[] = {
        {"S: Save", "(custom file)", -700.0f},
        {"L: Load", "(custom file)", -550.0f},   
        {"I: Reload", "(initial map)", -400.0f},
        {"C: Clear", "(entire map)", -250.0f} 
    };

    // Command keys
    for (int i = 0; i < 4; i++) {
        GetNormalizedTextPosition(commands[i].x, uiY, &textX, &textY);
        AEGfxPrint(fontID, commands[i].label, textX, textY, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    uiY -= 30.0f;

    // Descriptions
    for (int i = 0; i < 4; i++) {
        GetNormalizedTextPosition(commands[i].x, uiY, &textX, &textY);
        AEGfxPrint(fontID, commands[i].desc, textX, textY, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f);
    }
}
void Quit_Editor(EditorState& editor) {
    editor.editorPaintReady = false;
    AEAudioStopGroup(EditorBGMGroup);
}