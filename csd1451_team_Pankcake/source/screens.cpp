/*!************************************************************************
  \file screens.cpp
  \author
  Yeo Kah Boon Joshua ( High Score Management, Win/Lose Screens, Navigation Screen )





  \brief
  This source file contains definitions of the stuff related to infographics.

**************************************************************************/



#include "screens.hpp"
#include "utils.hpp"
#include <string.h>
#include "AEEngine.h" 
#include "player.hpp"
#include "gunTypes.hpp"
#include "enemies.hpp"
#include "weapon.hpp"
#include <vector>
#include "map.hpp"
#include "events.hpp"
#include "sound.hpp"
#include "gameMode.hpp"

static AEGfxTexture* gDigiPenLogo = nullptr;

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

bool IsButtonHovered(const Button& btn) {
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);

    float cx = (float)mx - 800.0f;
    float cy = 450.0f - (float)my;

    return AreBoxesIntersecting(cx, cy, 1.0f, 1.0f,
        btn.x, btn.y, btn.w, btn.h) == 1;
}

void DrawButton(const Button& btn, AEGfxVertexList* mesh, s8 font) {

    bool hovered = IsButtonHovered(btn);

    // Slight scale effect upon hovering
    float drawW = hovered ? btn.w + 20.0f : btn.w;
    float drawH = hovered ? btn.h + 10.0f : btn.h;

    // Background color (slightly green tint on hover)
    if (hovered)
        AEGfxSetColorToMultiply(0.0f, 0.2f, 0.0f, 1.0f);
    else
        AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);

    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

    DrawSprite(mesh, btn.x, btn.y, drawW, drawH, 0.0f);

    // Text scaling
    float textScale = hovered ? 1.3f : 1.2f;

    float textWidth, textHeight;
    AEGfxGetPrintSize(font, btn.label, textScale, &textWidth, &textHeight);

    float nx = btn.x / 800.0f;
    float ny = btn.y / 450.0f;

    nx -= textWidth / 2;
    ny -= textHeight / 2;

    // Text color (brightens on hovering over button)
    if (hovered)
        AEGfxPrint(font, const_cast<char*>(btn.label), nx, ny, textScale, 0.4f, 1.0f, 0.4f, 1.0f);
    else
        AEGfxPrint(font, const_cast<char*>(btn.label), nx, ny, textScale, 0.0f, 1.0f, 0.0f, 1.0f);
}

void LoadSplashAssets()
{
    if (!gDigiPenLogo)
        gDigiPenLogo = AEGfxTextureLoad("Assets/DigiPen_Singapore_WEB_WHITE.png");
}

void FreeSplashAssets()
{
    if (gDigiPenLogo) {
        AEGfxTextureUnload(gDigiPenLogo);
        gDigiPenLogo = nullptr;
    }
}

static void DrawConfirmWindow(
    AEGfxVertexList* mesh,
    s8 font,
    const char* title,
    const char* desc,
    const Button& yesBtn,
    const Button& noBtn)
{
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(CONFIRM_OVERLAY_ALPHA);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    DrawSprite(mesh, 0.0f, 0.0f, 1600.0f, 900.0f, 0.0f);

    AEGfxSetTransparency(CONFIRM_SHADOW_ALPHA);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    DrawSprite(mesh, CONFIRM_SHADOW_OFFSET, -CONFIRM_SHADOW_OFFSET, CONFIRM_SHADOW_W, CONFIRM_SHADOW_H, 0.0f);

    AEGfxSetTransparency(CONFIRM_PANEL_ALPHA);
    AEGfxSetColorToMultiply(0.04f, 0.04f, 0.07f, 1.0f);
    DrawSprite(mesh, 0.0f, 0.0f, CONFIRM_PANEL_W, CONFIRM_PANEL_H, 0.0f);

    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(0.35f, 0.08f, 0.08f, 1.0f);
    DrawSprite(mesh, 0.0f, CONFIRM_HEADER_Y, CONFIRM_HEADER_W, CONFIRM_HEADER_H, 0.0f);

    float tw, th;

    AEGfxGetPrintSize(font, title, CONFIRM_TITLE_SCALE, &tw, &th);
    AEGfxPrint(font, const_cast<char*>(title), -tw / 2.0f, 0.15f, CONFIRM_TITLE_SCALE, 1.0f, 0.35f, 0.35f, 1.0f);

    AEGfxGetPrintSize(font, desc, CONFIRM_DESC_SCALE, &tw, &th);
    AEGfxPrint(font, const_cast<char*>(desc), -tw / 2.0f - 0.01f, 0.00f, CONFIRM_DESC_RENDER_SCALE, 1.0f, 1.0f, 1.0f, 1.0f);

    DrawButton(yesBtn, mesh, font);
    DrawButton(noBtn, mesh, font);
}

GameState UpdateAndRenderSplash(AEGfxVertexList* mesh, float dt)
{
    static float splashTimer = 0.0f;
    splashTimer += dt;

    const float fadeInTime = SPLASH_FADE_IN_TIME;
    const float visibleTime = SPLASH_VISIBLE_TIME;
    const float fadeOutTime = SPLASH_FADE_OUT_TIME;
    const float totalTime = fadeInTime + visibleTime + fadeOutTime;

    float alpha = 1.0f;

    if (splashTimer <= fadeInTime) {
        alpha = splashTimer / fadeInTime;
    }
    else if (splashTimer >= fadeInTime + visibleTime) {
        alpha = (totalTime - splashTimer) / fadeOutTime;
    }

    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;

    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(alpha);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

    if (gDigiPenLogo) {
        AEGfxTextureSet(gDigiPenLogo, 0.0f, 0.0f);
        DrawSprite(mesh, 0.0f, 0.0f, SPLASH_LOGO_W, SPLASH_LOGO_H, 0.0f);
    }

    AEGfxSetTransparency(1.0f);

    if (splashTimer >= totalTime) {
        splashTimer = 0.0f;
        return STATE_MENU;
    }

    return STATE_DIGIPENLOGO;
}

GameState UpdateAndRenderMenu(AEGfxVertexList* mesh, s8 font, float dt) {
    static float menuAnimTime = 0.0f;
    menuAnimTime += dt;

    static bool initialized = false;
    static ConfirmAction confirmAction = CONFIRM_NONE;

    struct MenuBlock {
        float x, y;
        float size;
        float speedX;
    };

    static MenuBlock blocks[MENU_BG_BLOCK_COUNT];

    if (!initialized) {
        for (int i = 0; i < MENU_BG_BLOCK_COUNT; ++i) {
            blocks[i].size = MENU_BLOCK_MIN_SIZE + (rand() % MENU_BLOCK_SIZE_RAND);
            blocks[i].x = -900.0f - (rand() % 500) - i * MENU_BLOCK_START_X_OFFSET;
            blocks[i].y = (rand() % 900) - 450.0f;
            blocks[i].speedX = MENU_BLOCK_MIN_SPEED + (rand() % MENU_BLOCK_SPEED_RAND);
        }
        initialized = true;
    }

    float pulseScale = MENU_PULSE_BASE + MENU_PULSE_AMP * sinf(menuAnimTime * MENU_PULSE_FREQ);

    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    for (int i = 0; i < MENU_BG_BLOCK_COUNT; ++i) {
        blocks[i].x += blocks[i].speedX * dt;

        if (blocks[i].x - blocks[i].size > 900.0f) {
            blocks[i].size = MENU_BLOCK_MIN_SIZE + (rand() % MENU_BLOCK_SIZE_RAND);
            blocks[i].x = -900.0f - (rand() % 500);
            blocks[i].y = (rand() % 900) - 450.0f;
            blocks[i].speedX = MENU_BLOCK_MIN_SPEED + (rand() % MENU_BLOCK_SPEED_RAND);
        }

        float s = blocks[i].size;
        AEGfxSetColorToMultiply(0.15f, 0.9f, 0.15f, 0.45f);
        AEGfxSetColorToAdd(0, 0, 0, 0);
        DrawSprite(mesh, blocks[i].x, blocks[i].y, s, s, 0.0f);
    }

    float tw, th;
    AEGfxGetPrintSize(font, "Flag Protocol", pulseScale, &tw, &th);

    float titleX = -tw / 2.0f;
    float titleY = 0.65f;
    AEGfxPrint(font, const_cast<char*>("Flag Protocol"), titleX, titleY, pulseScale, 0.0f, 1.0f, 0.0f, 1.0f);

    Button playBtn = { 0.0f,  200.0f, 200.0f, 50.0f, "PLAY" };
    Button controlsBtn = { 0.0f,  100.0f, 200.0f, 50.0f, "CONTROLS" };
    Button creditBtn = { 0.0f,    0.0f, 200.0f, 50.0f, "CREDITS" };
    Button editorBtn = { 0.0f, -100.0f, 200.0f, 50.0f, "EDITOR" };
    Button settingsBtn = { 0.0f, -200.0f, 200.0f, 50.0f, "SETTINGS" };
    Button quitBtn = { 0.0f, -320.0f, 200.0f, 50.0f, "QUIT" };

    DrawButton(playBtn, mesh, font);
    DrawButton(controlsBtn, mesh, font);
    DrawButton(creditBtn, mesh, font);
    DrawButton(editorBtn, mesh, font);
    DrawButton(settingsBtn, mesh, font);
    DrawButton(quitBtn, mesh, font);

    if (confirmAction == CONFIRM_NONE) {
        if (IsButtonClicked(playBtn))     return STATE_NAVIGATION;
        if (IsButtonClicked(controlsBtn)) return STATE_CONTROLS;
        if (IsButtonClicked(creditBtn))   return STATE_CREDITS;
        if (IsButtonClicked(editorBtn))   return STATE_EDITOR;
        if (IsButtonClicked(settingsBtn)) return STATE_SETTINGS;
        if (IsButtonClicked(quitBtn))     confirmAction = CONFIRM_QUIT;
    }

    static bool showScoreboard = false;

    Button scoreboardBtn = { SCOREBOARD_BTN_X, SCOREBOARD_BTN_Y, SCOREBOARD_BTN_W, SCOREBOARD_BTN_H, "SCOREBOARD" };
    DrawButton(scoreboardBtn, mesh, font);
    if (IsButtonClicked(scoreboardBtn))
        showScoreboard = !showScoreboard;

    if (showScoreboard) {
        float popX = SCOREBOARD_POP_X;
        float popY = SCOREBOARD_POP_Y;

        AEGfxSetTransparency(0.85f);
        AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
        AEGfxSetColorToAdd(0, 0, 0, 0);
        DrawSprite(mesh, popX, popY, SCOREBOARD_POP_W, SCOREBOARD_POP_H, 0.0f);
        AEGfxSetTransparency(1.0f);

        float popCenterNX = popX / (SCREEN_WIDTH / 2.0f);

        const char* topStr = "TOP SCORES:";
        AEGfxGetPrintSize(font, topStr, 0.9f, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(topStr), popCenterNX - tw / 2, (popY + 100.0f) / (SCREEN_HEIGHT / 2.0f), 0.9f, 1, 0.8f, 0, 1);

        for (int i = 0; i < SCOREBOARD_ENTRY_COUNT; i++) {
            char entry[64];
            sprintf_s(entry, "%d. %s  %d", i + 1, highScores[i].name, highScores[i].score);
            AEGfxGetPrintSize(font, entry, 0.8f, &tw, &th);
            AEGfxPrint(font, entry, popCenterNX - tw / 2,
                (popY + 50.0f - i * SCOREBOARD_ENTRY_STEP_Y) / (SCREEN_HEIGHT / 2.0f),
                0.8f, 1, 1, 1, 1);
        }
    }

    if (confirmAction == CONFIRM_QUIT) {
        Button yesBtn = { CONFIRM_BTN_YES_X, CONFIRM_BTN_Y, CONFIRM_BTN_W, CONFIRM_BTN_H, "YES" };
        Button noBtn = { CONFIRM_BTN_NO_X,  CONFIRM_BTN_Y, CONFIRM_BTN_W, CONFIRM_BTN_H, "NO" };

        DrawConfirmWindow(mesh, font,
            "QUIT GAME?",
            "Exit application completely?",
            yesBtn, noBtn);

        if (IsButtonClicked(yesBtn)) {
            confirmAction = CONFIRM_NONE;
            return STATE_QUIT;
        }
        if (IsButtonClicked(noBtn) || AEInputCheckTriggered(AEVK_ESCAPE)) {
            confirmAction = CONFIRM_NONE;
        }
    }

    return STATE_MENU;
}

GameState UpdateAndRenderPause(AEGfxVertexList* mesh, s8 font, float dt, GameState previous) {
    (void)dt;

    static ConfirmAction confirmAction = CONFIRM_NONE;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(PAUSE_OVERLAY_ALPHA);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    DrawSprite(mesh, 0.0f, 0.0f, 1600.0f, 900.0f, 0.0f);

    AEGfxSetTransparency(1.0f);

    float tx, ty;
    float titleWidth, titleHeight;
    AEGfxGetPrintSize(font, "PAUSED", PAUSE_TITLE_SCALE, &titleWidth, &titleHeight);

    tx = -titleWidth / 2;
    ty = PAUSE_TITLE_Y;
    AEGfxPrint(font, const_cast<char*>("PAUSED"), tx, ty, PAUSE_TITLE_SCALE, 1.0f, 1.0f, 1.0f, 1.0f);

    Button resumeBtn = { 0.0f,  100.0f, 200.0f, 50.0f, "RESUME" };
    Button menuBtn = { 0.0f,    0.0f, 200.0f, 50.0f, "MENU" };
    Button quitBtn = { 0.0f, -100.0f, 200.0f, 50.0f, "QUIT" };

    DrawButton(resumeBtn, mesh, font);
    DrawButton(menuBtn, mesh, font);
    DrawButton(quitBtn, mesh, font);
    DrawControlsHint(fontID);

    if (confirmAction == CONFIRM_NONE) {
        if (AEInputCheckTriggered(AEVK_P) || AEInputCheckTriggered(AEVK_ESCAPE))
            return previous;
        if (IsButtonClicked(resumeBtn)) return previous;
        if (IsButtonClicked(menuBtn))   confirmAction = CONFIRM_MENU;
        if (IsButtonClicked(quitBtn))   confirmAction = CONFIRM_QUIT;
    }

    if (confirmAction != CONFIRM_NONE) {
        Button yesBtn = { CONFIRM_BTN_YES_X, CONFIRM_BTN_Y, CONFIRM_BTN_W, CONFIRM_BTN_H, "YES" };
        Button noBtn = { CONFIRM_BTN_NO_X,  CONFIRM_BTN_Y, CONFIRM_BTN_W, CONFIRM_BTN_H, "NO" };

        if (confirmAction == CONFIRM_MENU) {
            DrawConfirmWindow(mesh, font,
                "RETURN TO MAIN MENU?",
                "Progress will be lost.",
                yesBtn, noBtn);

            if (IsButtonClicked(yesBtn)) {
                confirmAction = CONFIRM_NONE;
                AEAudioStopGroup(GameBGMGroup);
                AEAudioStopGroup(EditorBGMGroup);
                return STATE_MENU;
            }
            if (IsButtonClicked(noBtn) || AEInputCheckTriggered(AEVK_ESCAPE))
                confirmAction = CONFIRM_NONE;
        }

        if (confirmAction == CONFIRM_QUIT) {
            DrawConfirmWindow(mesh, font,
                "QUIT GAME?",
                "Exit application completely?",
                yesBtn, noBtn);

            if (IsButtonClicked(yesBtn)) {
                confirmAction = CONFIRM_NONE;
                return STATE_QUIT;
            }
            if (IsButtonClicked(noBtn) || AEInputCheckTriggered(AEVK_ESCAPE))
                confirmAction = CONFIRM_NONE;
        }
    }

    return STATE_PAUSE;
}

GameState UpdateAndRenderControls(AEGfxVertexList* mesh, s8 font, float dt) {
    (void)dt;

    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    float textX, textY;

    // Title
    float tw, th;
    AEGfxGetPrintSize(font, "CONTROLS", 1.8f, &tw, &th);
    AEGfxPrint(font, const_cast<char*>("CONTROLS"), -tw / 2.0f, 0.75f, 1.8f, 0.0f, 1.0f, 0.0f, 1.0f);

    // Movement
    GetNormalizedTextPosition(-620.0f, 240.0f, &textX, &textY);
    AEGfxPrint(font, const_cast<char*>("MOVEMENT"), textX, textY, 1.2f, 0.0f, 1.0f, 0.8f, 1.0f);

    float xKey, y;
    float xDesc, unusedY;
    GetNormalizedTextPosition(-520.0f, 0.0f, &xDesc, &unusedY);

    // Directional movement
    GetNormalizedTextPosition(-620.0f, 190.0f, &xKey, &y);
    AEGfxPrint(font, "W A S D", xKey, y, 1.0f, 1, 1, 1, 1);
    AEGfxPrint(font, "- Move", xDesc, y, 1.0f, 1, 1, 1, 1);

    // Sprint
    GetNormalizedTextPosition(-620.0f, 150.0f, &xKey, &y);
    AEGfxPrint(font, "RMB", xKey, y, 1.0f, 1, 1, 1, 1);
    AEGfxPrint(font, "- Sprint", xDesc, y, 1.0f, 1, 1, 1, 1);

    // Dash
    GetNormalizedTextPosition(-620.0f, 110.0f, &xKey, &y);
    AEGfxPrint(font, "SPACE", xKey, y, 1.0f, 1, 1, 1, 1);
    AEGfxPrint(font, "- Dash (costs health)", xDesc, y, 1.0f, 1, 1, 1, 1);

    // Combat
    GetNormalizedTextPosition(-620.0f, 30.0f, &textX, &textY);
    AEGfxPrint(font, const_cast<char*>("COMBAT"), textX, textY, 1.2f, 1.0f, 0.9f, 0.3f, 1.0f);

    // Shoot
    GetNormalizedTextPosition(-620.0f, -20.0f, &xKey, &y);
    AEGfxPrint(font, "LMB", xKey, y, 1.0f, 1, 1, 1, 1);
    AEGfxPrint(font, "- Shoot", xDesc, y, 1.0f, 1, 1, 1, 1);

    // Reload
    GetNormalizedTextPosition(-330.0f, -60.0f, &xKey, &y);
    AEGfxPrint(font, "R", xKey, y, 1.0f, 1, 1, 1, 1);
    AEGfxPrint(font, "- Reload", xDesc, y, 1.0f, 1, 1, 1, 1);

    // Switch weapons
    GetNormalizedTextPosition(-620.0f, -100.0f, &xKey, &y);
    AEGfxPrint(font, "1 / 2 / 3 or MouseScroll", xKey, y, 1.0f, 1, 1, 1, 1);

    float xDescWide, unusedWide;
    GetNormalizedTextPosition(-330.0f, 0.0f, &xDescWide, &unusedWide);
    AEGfxPrint(font, "- Switch Weapons", xDescWide, y, 1.0f, 1, 1, 1, 1);

    // Objective
    GetNormalizedTextPosition(120.0f, 240.0f, &textX, &textY);
    AEGfxPrint(font, const_cast<char*>("OBJECTIVE"), textX, textY, 1.2f, 1.0f, 0.4f, 1.0f, 1.0f);

    GetNormalizedTextPosition(120.0f, 190.0f, &textX, &textY);
    AEGfxPrint(font, const_cast<char*>("Step on CYAN / LIME tiles"), textX, textY, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f);

    GetNormalizedTextPosition(120.0f, 150.0f, &textX, &textY);
    AEGfxPrint(font, const_cast<char*>("to pick up FLAG NODES."), textX, textY, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f);

    GetNormalizedTextPosition(120.0f, 90.0f, &textX, &textY);
    AEGfxPrint(font, const_cast<char*>("Step on PURPLE tiles"), textX, textY, 1.0f, 0.7f, 0.3f, 1.0f, 1.0f);

    GetNormalizedTextPosition(120.0f, 50.0f, &textX, &textY);
    AEGfxPrint(font, const_cast<char*>("to deliver them for points."), textX, textY, 1.0f, 0.7f, 0.3f, 1.0f, 1.0f);

    GetNormalizedTextPosition(120.0f, -10.0f, &textX, &textY);
    AEGfxPrint(font, const_cast<char*>("Capture 10 FLAG NODES to win."), textX, textY, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    float xKeyR, yR;
    float xDescR, unusedYR;
    GetNormalizedTextPosition(240.0f, 0.0f, &xDescR, &unusedYR);

    // Navigation
    GetNormalizedTextPosition(120.0f, -100.0f, &textX, &textY);
    AEGfxPrint(font, const_cast<char*>("NAVIGATION"), textX, textY, 1.2f, 0.4f, 1.0f, 0.4f, 1.0f);

    // P / ESC
    GetNormalizedTextPosition(120.0f, -150.0f, &xKeyR, &yR);
    AEGfxPrint(font, "P / ESC", xKeyR, yR, 1.0f, 1, 1, 1, 1);
    AEGfxPrint(font, "- Pause", xDescR, yR, 1.0f, 1, 1, 1, 1);

    // Q
    GetNormalizedTextPosition(120.0f, -190.0f, &xKeyR, &yR);
    AEGfxPrint(font, "Q", xKeyR, yR, 1.0f, 1, 1, 1, 1);
    AEGfxPrint(font, "- Return / Quit", xDescR, yR, 1.0f, 1, 1, 1, 1);

    // Back button at bottom
    Button backBtn = { 0.0f, -320.0f, 220.0f, 55.0f, "BACK" };
    DrawButton(backBtn, mesh, font);

    if (IsButtonClicked(backBtn) || AEInputCheckTriggered(AEVK_Q) || AEInputCheckTriggered(AEVK_ESCAPE)) {
        return STATE_MENU;
    }

    return STATE_CONTROLS;
}

GameState UpdateAndRenderNavigation(AEGfxVertexList* mesh, s8 font, float dt) {
    (void)dt;

    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    float tx, ty;

    // Title
    GetNormalizedTextPosition(-110.0f, 350.0f, &tx, &ty);
    AEGfxPrint(font, const_cast<char*>("SELECT GAME MODE"), tx, ty, 1.5f, 0.0f, 1.0f, 0.0f, 1.0f);

    // Navigation buttons
    Button playBtn = { 0.0f,  200.0f, 250.0f, 60.0f, "MAIN GAME" };
    Button trainingBtn = { 0.0f,   50.0f, 250.0f, 60.0f, "TRAINING" };
    Button customBtn = { 0.0f,  -100.0f, 250.0f, 60.0f, "CUSTOM LEVEL" };
    Button backBtn = { 0.0f, -300.0f, 280.0f, 60.0f, "BACK TO MAIN MENU" };

    DrawButton(playBtn, mesh, font);
    DrawButton(trainingBtn, mesh, font);
    DrawButton(customBtn, mesh, font);
    DrawButton(backBtn, mesh, font);

    // Button interactions
    if (IsButtonClicked(playBtn)) return STATE_CUTSCENE;
    if (IsButtonClicked(trainingBtn)) return STATE_TUTORIAL; //Placeholder
    if (IsButtonClicked(customBtn)) return STATE_CUSTOM; // Placeholder
    if (IsButtonClicked(backBtn)) return STATE_MENU;

    return STATE_NAVIGATION;
}

GameState UpdateAndRenderWinScreen(AEGfxVertexList* mesh, s8 font, float dt, Player& player, GameState previous) {
    (void)dt;
    AEGfxSetBackgroundColor(WIN_BG_R, WIN_BG_G, WIN_BG_B);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    float tw, th;

    AEGfxGetPrintSize(font, "YOU WIN!", ENDSCREEN_TITLE_SCALE, &tw, &th);
    AEGfxPrint(font, const_cast<char*>("YOU WIN!"), -tw / 2, ENDSCREEN_TITLE_Y, ENDSCREEN_TITLE_SCALE, 0.0f, 1.0f, 0.0f, 1.0f);

    char scoreStr[64], gruntsStr[64], flagStr[64];
    sprintf_s(scoreStr, "SCORE: %d", player.score);
    sprintf_s(gruntsStr, "VIRUSES KILLED: %d", player.gruntsKilled);
    sprintf_s(flagStr, "FLAGS CAPTURED: %d", player.flagCaptures);

    AEGfxGetPrintSize(font, scoreStr, ENDSCREEN_STATS_SCALE, &tw, &th);
    AEGfxPrint(font, scoreStr, -tw / 2, ENDSCREEN_STAT_SCORE_Y, ENDSCREEN_STATS_SCALE, 1, 1, 1, 1);
    AEGfxGetPrintSize(font, gruntsStr, ENDSCREEN_STATS_SCALE, &tw, &th);
    AEGfxPrint(font, gruntsStr, -tw / 2, ENDSCREEN_STAT_GRUNTS_Y, ENDSCREEN_STATS_SCALE, 1, 1, 1, 1);
    AEGfxGetPrintSize(font, flagStr, ENDSCREEN_STATS_SCALE, &tw, &th);
    AEGfxPrint(font, flagStr, -tw / 2, ENDSCREEN_STAT_FLAGS_Y, ENDSCREEN_STATS_SCALE, 1, 1, 1, 1);

    bool fromMainGame = (previous == STATE_GAME);

    if (fromMainGame) {
        if (IsTopThree(player.score) && !scoreSaved) {
            const char* promptStr = "TOP 3 SCORE! ENTER YOUR NAME:";
            AEGfxGetPrintSize(font, promptStr, WIN_PROMPT_SCALE, &tw, &th);
            AEGfxPrint(font, const_cast<char*>(promptStr), -tw / 2, WIN_PROMPT_Y, WIN_PROMPT_SCALE, 1, 0.8f, 0, 1);

            for (u8 k = AEVK_A; k <= AEVK_Z; k++) {
                if (AEInputCheckTriggered(k) && nameInputLen < WIN_NAME_INPUT_MAX) {
                    nameInputBuffer[nameInputLen++] = 'A' + (k - AEVK_A);
                    nameInputBuffer[nameInputLen] = '\0';
                }
            }
            if (AEInputCheckTriggered(AEVK_BACK) && nameInputLen > 0)
                nameInputBuffer[--nameInputLen] = '\0';

            char displayName[64];
            sprintf_s(displayName, "> %s_", nameInputBuffer);
            AEGfxGetPrintSize(font, displayName, WIN_NAME_DISPLAY_SCALE, &tw, &th);
            AEGfxPrint(font, displayName, -tw / 2, WIN_NAME_DISPLAY_Y, WIN_NAME_DISPLAY_SCALE, 0, 1, 1, 1);

            if (nameInputLen > 0) {
                Button saveBtn = { WIN_SAVE_BTN_X, WIN_SAVE_BTN_Y, ENDSCREEN_BTN_W, ENDSCREEN_BTN_H, "SAVE SCORE" };
                DrawButton(saveBtn, mesh, font);
                if (IsButtonClicked(saveBtn)) {
                    InsertHighScore(nameInputBuffer, player.score);
                    scoreSaved = true;
                    nameInputLen = 0;
                    nameInputBuffer[0] = '\0';
                }
            }
        }
        else {
            const char* topStr = "TOP SCORES:";
            AEGfxGetPrintSize(font, topStr, WIN_TOPSCORE_SCALE, &tw, &th);
            AEGfxPrint(font, const_cast<char*>(topStr), -tw / 2, WIN_TOPSCORE_Y, WIN_TOPSCORE_SCALE, 1, 0.8f, 0, 1);

            for (int i = 0; i < SCOREBOARD_ENTRY_COUNT; i++) {
                char entry[64];
                sprintf_s(entry, "%d. %s  %d", i + 1, highScores[i].name, highScores[i].score);
                AEGfxGetPrintSize(font, entry, WIN_ENTRY_SCALE, &tw, &th);
                AEGfxPrint(font, entry, -tw / 2, WIN_ENTRY_START_Y - i * WIN_ENTRY_STEP_Y, WIN_ENTRY_SCALE, 1, 1, 1, 1);
            }
        }
    }

    float btnY = fromMainGame ? WIN_NAV_BTN_Y_MAIN : WIN_NAV_BTN_Y_DEFAULT;
    Button menuBtn = { WIN_NAV_BTN_LEFT_X,  btnY, ENDSCREEN_BTN_W, ENDSCREEN_BTN_H, "MAIN MENU" };
    Button creditsBtn = { WIN_NAV_BTN_RIGHT_X, btnY, ENDSCREEN_BTN_W, ENDSCREEN_BTN_H, "CREDITS" };
    DrawButton(menuBtn, mesh, font);
    DrawButton(creditsBtn, mesh, font);

    if (IsButtonClicked(menuBtn)) {
        scoreSaved = false;
        AEAudioStopGroup(WinBGMGroup);
        AEAudioStopGroup(GameBGMGroup);
        return STATE_MENU;
    }
    if (IsButtonClicked(creditsBtn)) {
        scoreSaved = false;
        AEAudioStopGroup(WinBGMGroup);
        AEAudioStopGroup(GameBGMGroup);
        return STATE_CREDITS;
    }

    return STATE_WIN;
}


GameState UpdateAndRenderLoseScreen(AEGfxVertexList* mesh, s8 font, float dt,
    Player& player, GameState previous) {
    (void)dt;

    AEGfxSetBackgroundColor(LOSE_BG_R, LOSE_BG_G, LOSE_BG_B);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    float tw, th;

    AEGfxGetPrintSize(font, "YOU DIED!", ENDSCREEN_TITLE_SCALE, &tw, &th);
    AEGfxPrint(font, const_cast<char*>("YOU DIED!"), -tw / 2, ENDSCREEN_TITLE_Y, ENDSCREEN_TITLE_SCALE, 1.0f, 0.0f, 0.0f, 1.0f);

    char scoreStr[64], gruntsStr[64], flagStr[64];
    sprintf_s(scoreStr, "SCORE: %d", player.score);
    sprintf_s(gruntsStr, "VIRUSES KILLED: %d", player.gruntsKilled);
    sprintf_s(flagStr, "FLAGS CAPTURED: %d", player.flagCaptures);

    AEGfxGetPrintSize(font, scoreStr, ENDSCREEN_STATS_SCALE, &tw, &th);
    AEGfxPrint(font, scoreStr, -tw / 2, ENDSCREEN_STAT_SCORE_Y, ENDSCREEN_STATS_SCALE, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxGetPrintSize(font, gruntsStr, ENDSCREEN_STATS_SCALE, &tw, &th);
    AEGfxPrint(font, gruntsStr, -tw / 2, ENDSCREEN_STAT_GRUNTS_Y, ENDSCREEN_STATS_SCALE, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxGetPrintSize(font, flagStr, ENDSCREEN_STATS_SCALE, &tw, &th);
    AEGfxPrint(font, flagStr, -tw / 2, ENDSCREEN_STAT_FLAGS_Y, ENDSCREEN_STATS_SCALE, 1.0f, 1.0f, 1.0f, 1.0f);

    Button retryBtn = { LOSE_RETRY_BTN_X, LOSE_RETRY_BTN_Y, ENDSCREEN_BTN_W, ENDSCREEN_BTN_H, "RETRY" };
    Button menuBtn = { LOSE_MENU_BTN_X,  LOSE_MENU_BTN_Y,  ENDSCREEN_BTN_W, ENDSCREEN_BTN_H, "MAIN MENU" };

    DrawButton(retryBtn, mesh, font);
    DrawButton(menuBtn, mesh, font);

    if (IsButtonClicked(retryBtn)) {
        LoadPlayer(player);
        AEAudioStopGroup(LoseBGMGroup);
        AEAudioPlay(GameBGM1, GameBGMGroup, 1.0f, 1.0f, -1);
        return previous;
    }
    if (IsButtonClicked(menuBtn)) {
        AEAudioStopGroup(LoseBGMGroup);
        AEAudioStopGroup(GameBGMGroup);
        return STATE_MENU;
    }

    return STATE_LOSE;
}


GameState UpdateAndRenderCredits(AEGfxVertexList* mesh, s8 font, float dt) {
    (void)dt;

    static int creditsPage = 0;

    AEGfxSetBackgroundColor(WIN_BG_R, WIN_BG_G, WIN_BG_B);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    float tw, th;

    AEGfxGetPrintSize(font, "Credits", CREDITS_TITLE_SCALE, &tw, &th);
    AEGfxPrint(font, const_cast<char*>("Credits"), -tw / 2.0f, CREDITS_TITLE_Y, CREDITS_TITLE_SCALE, 1.0f, 0.92f, 0.92f, 1.0f);

    switch (creditsPage) {
    case 0:
    {
        const char* line1 = "WWW.DIGIPEN.EDU";
        const char* line2 = "All content (C) 2026 DigiPen Institute of Technology Singapore";
        const char* line3 = "All Rights Reserved.";

        AEGfxGetPrintSize(font, line1, CREDITS_BODY_SCALE, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(line1), -tw / 2.0f, CREDITS_P0_LINE1_Y, CREDITS_BODY_SCALE, 1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxGetPrintSize(font, line2, CREDITS_BODY_SCALE_MED, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(line2), -tw / 2.0f, CREDITS_P0_LINE2_Y, CREDITS_BODY_SCALE_MED, 1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxGetPrintSize(font, line3, CREDITS_BODY_SCALE_MED, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(line3), -tw / 2.0f, CREDITS_P0_LINE3_Y, CREDITS_BODY_SCALE_MED, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    break;

    case 1:
    {
        const char* header = "TEAM MEMBERS";
        const char* names[CREDITS_P1_NAME_COUNT] = {
            "Yeo Kah Boon Joshua",
            "Ye Min Thwin",
            "Zhang Yida",
            "Pang Jun Yuan"
        };

        AEGfxGetPrintSize(font, header, CREDITS_HEADER_SCALE, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(header), -tw / 2.0f, CREDITS_P1_HEADER_Y, CREDITS_HEADER_SCALE, 0.0f, 1.0f, 0.8f, 1.0f);

        for (int i = 0; i < CREDITS_P1_NAME_COUNT; ++i) {
            AEGfxGetPrintSize(font, names[i], CREDITS_BODY_SCALE, &tw, &th);
            AEGfxPrint(font, const_cast<char*>(names[i]), -tw / 2.0f, CREDITS_P1_NAMES_START_Y - i * CREDITS_P1_NAMES_STEP_Y, CREDITS_BODY_SCALE, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
    break;

    case 2:
    {
        const char* header = "FACULTY & INSTRUCTORS";
        const char* presidentHeader = "PRESIDENT";
        const char* presidentName = "Claude Comair";
        const char* instructorHeader = "INSTRUCTORS";
        const char* instructors[CREDITS_P2_INSTRUCTOR_COUNT] = {
            "Gerald Wong Han Feng",
            "Dr. Soroor Malekmohammadai Faradounbeh",
            "Tommy Tan Chee Wei"
        };

        AEGfxGetPrintSize(font, header, CREDITS_HEADER_SCALE, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(header), -tw / 2.0f, CREDITS_P2_HEADER_Y, CREDITS_HEADER_SCALE, 0.0f, 1.0f, 0.8f, 1.0f);

        AEGfxGetPrintSize(font, presidentHeader, CREDITS_BODY_SCALE_MED, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(presidentHeader), -tw / 2.0f, CREDITS_P2_PRESIDENT_HDR_Y, CREDITS_BODY_SCALE_MED, 1.0f, 0.9f, 0.3f, 1.0f);

        AEGfxGetPrintSize(font, presidentName, CREDITS_BODY_SCALE_MED, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(presidentName), -tw / 2.0f, CREDITS_P2_PRESIDENT_NAME_Y, CREDITS_BODY_SCALE_MED, 1.0f, 1.0f, 1.0f, 1.0f);

        AEGfxGetPrintSize(font, instructorHeader, CREDITS_BODY_SCALE_MED, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(instructorHeader), -tw / 2.0f, CREDITS_P2_INSTRUCTOR_HDR_Y, CREDITS_BODY_SCALE_MED, 1.0f, 0.9f, 0.3f, 1.0f);

        for (int i = 0; i < CREDITS_P2_INSTRUCTOR_COUNT; ++i) {
            AEGfxGetPrintSize(font, instructors[i], CREDITS_BODY_SCALE_SM, &tw, &th);
            AEGfxPrint(font, const_cast<char*>(instructors[i]), -tw / 2.0f, CREDITS_P2_INSTRUCTOR_START_Y - i * CREDITS_P2_INSTRUCTOR_STEP_Y, CREDITS_BODY_SCALE_SM, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
    break;

    case 3:
    {
        const char* header = "SPECIAL THANKS ;)";
        const char* thanks[CREDITS_P3_THANKS_COUNT] = {
            "Our instructors",
            "Our classmates",
            "Our playtesters"
        };

        AEGfxGetPrintSize(font, header, CREDITS_HEADER_SCALE, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(header), -tw / 2.0f, CREDITS_P3_HEADER_Y, CREDITS_HEADER_SCALE, 0.0f, 1.0f, 0.8f, 1.0f);

        for (int i = 0; i < CREDITS_P3_THANKS_COUNT; ++i) {
            AEGfxGetPrintSize(font, thanks[i], CREDITS_BODY_SCALE, &tw, &th);
            AEGfxPrint(font, const_cast<char*>(thanks[i]), -tw / 2.0f, CREDITS_P3_THANKS_START_Y - i * CREDITS_P3_THANKS_STEP_Y, CREDITS_BODY_SCALE, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
    break;
    }

    char pageText[32];
    sprintf_s(pageText, "Page %d / %d", creditsPage + 1, CREDITS_TOTAL_PAGES);
    AEGfxGetPrintSize(font, pageText, CREDITS_PAGE_IND_SCALE, &tw, &th);
    AEGfxPrint(font, pageText, -tw / 2.0f, CREDITS_PAGE_IND_Y, CREDITS_PAGE_IND_SCALE, 1.0f, 0.92f, 0.92f, 1.0f);

    Button prevBtn = { CREDITS_PREV_BTN_X, CREDITS_NAV_BTN_Y, CREDITS_PREV_NEXT_BTN_W, CREDITS_NAV_BTN_H, "<<" };
    Button backBtn = { CREDITS_BACK_BTN_X, CREDITS_NAV_BTN_Y, CREDITS_BACK_BTN_W,      CREDITS_NAV_BTN_H, "Back" };
    Button nextBtn = { CREDITS_NEXT_BTN_X, CREDITS_NAV_BTN_Y, CREDITS_PREV_NEXT_BTN_W, CREDITS_NAV_BTN_H, ">>" };

    DrawButton(prevBtn, mesh, font);
    DrawButton(backBtn, mesh, font);
    DrawButton(nextBtn, mesh, font);

    if ((IsButtonClicked(prevBtn) || AEInputCheckTriggered(AEVK_LEFT)) && creditsPage > 0)
        creditsPage--;

    if ((IsButtonClicked(nextBtn) || AEInputCheckTriggered(AEVK_RIGHT)) && creditsPage < CREDITS_TOTAL_PAGES - 1)
        creditsPage++;

    if (IsButtonClicked(backBtn) || AEInputCheckTriggered(AEVK_ESCAPE)) {
        creditsPage = 0;
        return STATE_MENU;
    }

    return STATE_CREDITS;
}




void CurrGamestate(GameState state, int& gGameRunning, std::vector<std::vector<int>>& gameMap, Player& player, GameState previousState) {
    if (state == STATE_MENU) {
        LoadPlayer(player);
        ShowCursor(true);
        AEAudioPlay(MainMenuBGM, MainBGMGroup, 1.0f, 1.0f, -1);

    }
    else if (state == STATE_NAVIGATION) {
        ShowCursor(true);
    }
    else if (state == STATE_GAME) {
        GameStateLoad(gGameRunning, gameMap);
        ShowCursor(false);
        if(previousState == STATE_CUTSCENE) {
            AEAudioStopGroup(StaticBGMGroup);
            
        }
        
        if (previousState != STATE_PAUSE) {
			ShowCursor(false);
            LoadPlayer(player);

            AEAudioPlay(GameBGM1, GameBGMGroup, 1.0f, 1.0f, -1);
        }

    }
    else if (state == STATE_TUTORIAL) {
        GameStateLoadTutorial(gGameRunning, gameMap);
        ShowCursor(false);
        if (previousState != STATE_PAUSE) {
            LoadPlayer(player);
            AEAudioPlay(GameBGM1, GameBGMGroup, 1.0f, 1.0f, -1);

        
        }

    }
    else if (state == STATE_CUSTOM) {
        LoadPlayer(player);

        GameStateLoadCustom(gGameRunning, gameMap);
        ShowCursor(false);
        if (previousState != STATE_PAUSE) {
            LoadPlayer(player);
            AEAudioPlay(GameBGM1, GameBGMGroup, 1.0f, 1.0f, -1);
        }


    }
    else if (state == STATE_EDITOR) {
        GameStateLoadCustom(gGameRunning, gameMap);
        ShowCursor(true);

    }
    else if (state == STATE_CUTSCENE) {
        AEAudioPlay(StaticSound, StaticBGMGroup, 0.2f, 1.0f, -1);
        ShowCursor(false);
    }
    else if (state == STATE_PAUSE) {
        ShowCursor(true);
    }
}

void GameStateLoad(int& gGameRunning, std::vector<std::vector<int>>& gameMap) {
    if (!Load_Game_Level("Assets/Level_1_Map.txt", gameMap)) {
        //printf("Failed to load map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        //printf("Map loaded successfully!\n");
    }
    Spawn_Flag(gameMap);
    floodfill(gameMap, 0, 0);
}

void GameStateLoadTutorial(int& gGameRunning, std::vector<std::vector<int>>& gameMap) {
    if (!Load_Game_Level("Assets/Tutorial.txt", gameMap)) {
        //printf("Failed to load tutorial map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        //printf("Tutorial map loaded successfully!\n");
    }
    Spawn_Flag(gameMap);
}

void GameStateLoadCustom(int& gGameRunning, std::vector<std::vector<int>>& gameMap) {
    if (!Load_Game_Level("Assets/My_Custom_Map.txt", gameMap)) {
        //printf("Failed to load custom map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        //printf("Custom map loaded successfully!\n");
    }
    Spawn_Flag(gameMap);
    floodfill(gameMap, 0, 0);
}
void GameStateLoadEditor(int& gGameRunning, std::vector<std::vector<int>>& gameMap) {
    static bool loaded = false;
    if (loaded) return;

    if (!Load_Game_Level("Assets/My_Custom_Map.txt", gameMap)) {
        //printf("Failed to load editor map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        //printf("Editor map loaded successfully!\n");
    }
    loaded = true;
}
// Loads the top 3 high scores from a file into the highScores array
void LoadHighScores() {
    FILE* f;
    fopen_s(&f, "Assets/highscores.txt", "r");
    if (!f) return;
    for (int i = 0; i < 3; i++) {
        if (fscanf_s(f, "%31s %d", highScores[i].name, 32, &highScores[i].score) != 2) {
            //printf("Loaded score %d: %s %d\n", i, highScores[i].name, highScores[i].score);


            break;
        }
    }
    fclose(f);
}
// Saves the current top 3 high scores to a file
void SaveHighScores() {
    
    FILE* f;
    fopen_s(&f, "Assets/highscores.txt", "w");
    if (!f) return;
    for (int i = 0; i < 3; i++)
        fprintf(f, "%s %d\n", highScores[i].name, highScores[i].score);
    fclose(f);
}
// Checks if a score is higher than the current 3rd place score
bool IsTopThree(int score) {
    return score > highScores[2].score;
}

// Inserts a new high score if it's in the top 3, shifting lower scores down
void InsertHighScore(const char* name, int score) {
    // Insert and sort descending
    for (int i = 0; i < 3; i++) {
        if (score > highScores[i].score) {
            // Shift down
            for (int j = 2; j > i; j--)
                highScores[j] = highScores[j - 1];
            strncpy_s(highScores[i].name, name, 31);
            highScores[i].score = score;
            break;
        }
    }
    SaveHighScores();
}