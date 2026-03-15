#include "screens.hpp"
#include "utils.hpp"
#include <string.h>
#include "AEEngine.h"
#include "utils.hpp"  
#include "player.hpp"
#include "gunTypes.hpp"
#include "enemies.hpp"
#include "weapon.hpp"
#include <vector>
#include "map.hpp"
#include "events.hpp"

bool IsButtonClicked(const Button& btn) {
    if (!AEInputCheckTriggered(AEVK_LBUTTON)) return false;

    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);

    // Convert cursor from top-left origin to centered coordinate system
    float cx = (float)mx - 800.0f;
    float cy = 450.0f - (float)my;

    // Reuse AreBoxesIntersecting from utils - cursor is a 1x1 box
    return AreBoxesIntersecting(cx, cy, 1.0f, 1.0f,
        btn.x, btn.y, btn.w, btn.h) == 1;
}

void DrawButton(const Button& btn, AEGfxVertexList* mesh, s8 font) {
    // Black button background
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    DrawSprite(mesh, btn.x, btn.y, btn.w, btn.h, 0.0f);

    // Text centering in the box
    float textScale = 1.2f;
    float textWidth, textHeight;
    AEGfxGetPrintSize(font, btn.label, textScale, &textWidth, &textHeight);

    float nx, ny;

    nx = btn.x / 800.0f;
    ny = btn.y / 450.0f;

    nx -= textWidth / 2;
    ny -= textHeight / 2;

    // Green text
    AEGfxPrint(font, const_cast<char*>(btn.label), nx, ny, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
}

GameState UpdateAndRenderMenu(AEGfxVertexList* mesh, s8 font, float dt) {
    //in case for future anims
    (void)dt;

    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    float tx, ty;
    GetNormalizedTextPosition(-100.0f, 300.0f, &tx, &ty);
    AEGfxPrint(font, const_cast<char*>("Flag Protocol"), tx, ty, 2.0f, 0.0f, 1.0f, 0.0f, 1.0f);

    Button playBtn = { 0.0f,  150.0f, 200.0f, 50.0f, "PLAY" };
    Button tutorialBtn = { 0.0f,   50.0f, 200.0f, 50.0f, "TRAINING" };
    Button loadBtn = { 0.0f,  -50.0f, 200.0f, 50.0f, "CUSTOM" };
    Button editorBtn = { 0.0f,  -150.0f, 200.0f, 50.0f, "EDITOR" };
    Button quitBtn = { 0.0f, -300.0f, 200.0f, 50.0f, "QUIT" };

    DrawButton(playBtn, mesh, font);
    DrawButton(tutorialBtn, mesh, font);
    DrawButton(loadBtn, mesh, font);
    DrawButton(editorBtn, mesh, font);
    DrawButton(quitBtn, mesh, font);

    if (IsButtonClicked(playBtn))   return STATE_CUTSCENE;
    if (IsButtonClicked(tutorialBtn))   return STATE_TUTORIAL;
    if (IsButtonClicked(loadBtn)) return STATE_LOAD;
    if (IsButtonClicked(editorBtn)) return STATE_EDITOR;
    if (IsButtonClicked(quitBtn))   return STATE_QUIT;


    return STATE_MENU;
}

GameState UpdateAndRenderPause(AEGfxVertexList* mesh, s8 font, float dt) {
    (void)dt;

    // Show cursor for pause menu
    ShowCursor(true);

    // Translucent dark overlay
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(0.6f);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    DrawSprite(mesh, 0.0f, 0.0f, 1600.0f, 900.0f, 0.0f);

    // Reset transparency
    AEGfxSetTransparency(1.0f);

    // "PAUSED" text
    float tx, ty;
    float titleScale = 2.0f;
    float titleWidth, titleHeight;
    AEGfxGetPrintSize(font, "PAUSED", titleScale, &titleWidth, &titleHeight);

    tx = -titleWidth / 2;
    ty = 0.65f;

    
    AEGfxPrint(font, const_cast<char*>("PAUSED"), tx, ty, titleScale, 1.0f, 1.0f, 1.0f, 1.0f);

    // Buttons
    Button resumeBtn = { 0.0f,  100.0f, 200.0f, 50.0f, "RESUME" };
    Button menuBtn = { 0.0f,    0.0f, 200.0f, 50.0f, "MENU" };
    Button quitBtn = { 0.0f, -100.0f, 200.0f, 50.0f, "QUIT" };


    DrawButton(resumeBtn, mesh, font);
    DrawButton(menuBtn, mesh, font);
    DrawButton(quitBtn, mesh, font);

    // Button interactions
    if (IsButtonClicked(resumeBtn)) {
        ShowCursor(false);
        return STATE_GAME;
    }
    if (IsButtonClicked(menuBtn)) {
        ShowCursor(false);
        return STATE_MENU;
    }
    if (IsButtonClicked(menuBtn)) {
        ShowCursor(false);
        return STATE_MENU;
    }
    if (IsButtonClicked(quitBtn)) {
        ShowCursor(false);
        return STATE_QUIT;
    }

    return STATE_PAUSE;
}

void CurrGamestate(GameState state, int& gGameRunning, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    if (state == STATE_MENU)
        while (ShowCursor(true) < 0);
    else if (state == STATE_GAME) {
        GameStateLoad(gGameRunning, gameMap);
        ShowCursor(false);
    }
    else if (state == STATE_TUTORIAL)
        GameStateLoadTutorial(gGameRunning, gameMap);
    else if (state == STATE_LOAD)
        GameStateLoadCustom(gGameRunning, gameMap);
    else if (state == STATE_EDITOR)
        GameStateLoadCustom(gGameRunning, gameMap);
    
}

void GameStateLoad(int& gGameRunning, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    if (!Load_Game_Level("Assets/Level_1_Map.txt", gameMap)) {
        printf("Failed to load map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        printf("Map loaded successfully!\n");
    }
    Spawn_Flag(gameMap);
}

void GameStateLoadTutorial(int& gGameRunning, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    if (!Load_Game_Level("Assets/Tutorial.txt", gameMap)) {
        printf("Failed to load tutorial map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        printf("Tutorial map loaded successfully!\n");
    }
    Spawn_Flag(gameMap);
}

void GameStateLoadCustom(int& gGameRunning, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    if (!Load_Game_Level("Assets/My_Custom_Map.txt", gameMap)) {
        printf("Failed to load custom map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        printf("Custom map loaded successfully!\n");
    }
    Spawn_Flag(gameMap);
}
void GameStateLoadEditor(int& gGameRunning, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap) {
    static bool loaded = false;
    if (loaded) return;

    if (!Load_Game_Level("Assets/Test_Map.txt", gameMap)) {
        printf("Failed to load editor map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        printf("Editor map loaded successfully!\n");
    }
    loaded = true;
}

//
//void GameStateInit() {
//}
//
//void GameStateFree() {
//}
//
//void GameStateUnload() {
//}