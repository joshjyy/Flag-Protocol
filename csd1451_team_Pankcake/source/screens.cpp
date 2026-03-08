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

    // Estimate text width to center it horizontally on the button
    float charWidth = 15.0f;
    float textOffsetX = (strlen(btn.label) * charWidth) * 0.5f;

    float nx, ny;
    GetNormalizedTextPosition(btn.x - textOffsetX, btn.y - 8.0f, &nx, &ny);

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
    Button tutorialBtn = { 0.0f,   50.0f, 200.0f, 50.0f, "TUTORIAL" };
    Button editorBtn = { 0.0f,  -50.0f, 200.0f, 50.0f, "EDITOR" };
    Button quitBtn = { 0.0f, -150.0f, 200.0f, 50.0f, "QUIT" };

    DrawButton(playBtn, mesh, font);
    DrawButton(tutorialBtn, mesh, font);
    DrawButton(editorBtn, mesh, font);
    DrawButton(quitBtn, mesh, font);

    if (IsButtonClicked(playBtn))   return STATE_GAME;
    if (IsButtonClicked(tutorialBtn))   return STATE_TUTORIAL;
    if (IsButtonClicked(editorBtn)) return STATE_EDITOR;
    if (IsButtonClicked(quitBtn))   return STATE_QUIT;


    return STATE_MENU;
}



//void GameStateLoad() {
//}
//
//void GameStateInit() {
//}
//
//void GameStateFree() {
//}
//
//void GameStateUnload() {
//}