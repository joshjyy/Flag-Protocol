#include "screens.hpp"
#include "utils.hpp"
#include <string.h>

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
    float textScale = 1.0f;    
    float textWidth, textHeight;
    AEGfxGetPrintSize(font, btn.label, textScale, &textWidth, &textHeight);

    float nx, ny;
    GetNormalizedTextPosition(btn.x, btn.y - (textHeight * 450.0f / 2), &nx, &ny);
    
    // Adjustment for text width according to word length
    nx -= textWidth / 2;

    // Green text
    AEGfxPrint(font, const_cast<char*>(btn.label), nx, ny, textScale, 0.0f, 1.0f, 0.0f, 1.0f);
}

GameState UpdateAndRenderMenu(AEGfxVertexList* mesh, s8 font, float dt) {
    //in case for future anims
    (void)dt;

    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    // Centering the title
    float tx, ty;
    float titleScale = 2.0f;

    float titleWidth, titleHeight;
    AEGfxGetPrintSize(font, "Flag Protocol", titleScale, &titleWidth, &titleHeight);

    // Horizontal centering 
    tx = -titleWidth / 2;
    ty = 0.65f;

    AEGfxPrint(font, const_cast<char*>("Flag Protocol"), tx, ty, titleScale, 0.0f, 1.0f, 0.0f, 1.0f);

    Button playBtn = { 0.0f,  120.0f, 320.0f, 80.0f, "PLAY" };
    Button editorBtn = { 0.0f,   20.0f, 320.0f, 80.0f, "EDITOR" };
    Button quitBtn = { 0.0f,  -80.0f, 320.0f, 80.0f, "QUIT" };

    DrawButton(playBtn, mesh, font);
    DrawButton(editorBtn, mesh, font);
    DrawButton(quitBtn, mesh, font);

    if (IsButtonClicked(playBtn))   return STATE_GAME;
    if (IsButtonClicked(editorBtn)) return STATE_EDITOR;

    return STATE_MENU;
}

bool IsQuitClicked() {
    Button quitBtn = { 0.0f, -100.0f, 200.0f, 50.0f, "QUIT" };
    return IsButtonClicked(quitBtn);
}

GameState UpdateAndRenderPause(AEGfxVertexList* mesh, s8 font, float dt) {
    (void)dt;

    // Paused screen background
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(0.8f);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    DrawSprite(mesh, 0.0f, 0.0f, 1600.0f, 900.0f, 0.0f);

    // Reset transparency for UI elements
    AEGfxSetTransparency(1.0f);

    // Pause title
    float tx, ty;
    float titleScale = 2.0f;

    float titleWidth, titleHeight;
    AEGfxGetPrintSize(font, "PAUSED", titleScale, &titleWidth, &titleHeight);

    tx = -titleWidth / 2;
    ty = 0.65f;

    AEGfxPrint(font, const_cast<char*>("PAUSED"), tx, ty, titleScale, 1.0f, 1.0f, 1.0f, 1.0f);

    // Buttons
    Button resumeBtn = { 0.0f,  120.0f, 320.0f, 80.0f, "RESUME" };
    Button quitBtn = { 0.0f,  -80.0f, 320.0f, 80.0f, "QUIT" };

    DrawButton(resumeBtn, mesh, font);
    DrawButton(quitBtn, mesh, font);

    if (IsButtonClicked(resumeBtn)) return STATE_GAME;
    if (IsButtonClicked(quitBtn)) return STATE_MENU;

    return STATE_PAUSE;
}