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
    // Darker fullscreen overlay
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(0.88f);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    DrawSprite(mesh, 0.0f, 0.0f, 1600.0f, 900.0f, 0.0f);

    // Main panel shadow
    AEGfxSetTransparency(0.55f);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    DrawSprite(mesh, 8.0f, -8.0f, 500.0f, 210.0f, 0.0f);

    // Main panel
    AEGfxSetTransparency(0.96f);
    AEGfxSetColorToMultiply(0.04f, 0.04f, 0.07f, 1.0f);
    DrawSprite(mesh, 0.0f, 0.0f, 560.0f, 250.0f, 0.0f);

    // Simple inner header strip5
    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(0.35f, 0.08f, 0.08f, 1.0f);
    DrawSprite(mesh, 0.0f, 78.0f, 560.0f, 34.0f, 0.0f);

    float tw, th;

    // Centered title
    AEGfxGetPrintSize(font, title, 1.3f, &tw, &th);
    AEGfxPrint(font, const_cast<char*>(title), -tw / 2.0f, 0.15f, 1.3f, 1.0f, 0.35f, 0.35f, 1.0f);

    // Centered description
    AEGfxGetPrintSize(font, desc, 0.82f, &tw, &th);
    AEGfxPrint(font, const_cast<char*>(desc), -tw / 2.0f - 0.01f, 0.00f, 0.98f, 1.0f, 1.0f, 1.0f, 1.0f);

    DrawButton(yesBtn, mesh, font);
    DrawButton(noBtn, mesh, font);
}

GameState UpdateAndRenderSplash(AEGfxVertexList* mesh, float dt)
{
    static float splashTimer = 0.0f;
    splashTimer += dt;

    const float fadeInTime = 0.5f;
    const float visibleTime = 2.0f;
    const float fadeOutTime = 0.5f;
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
        DrawSprite(mesh, 0.0f, 0.0f, 900.0f, 260.0f, 0.0f);
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

    static MenuBlock blocks[15];

    // block trailing effect in the background
    if (!initialized) {
        for (int i = 0; i < 15; ++i) {
            blocks[i].size = 18.0f + (rand() % 45);
            blocks[i].x = -900.0f - (rand() % 500) - i * 120.0f;
            blocks[i].y = (rand() % 900) - 450.0f;
            blocks[i].speedX = 450.0f + (rand() % 200);
        }
        initialized = true;
    }

    // pulsing effect for title
    float pulseScale = 2.5f + 0.2f * sinf(menuAnimTime * 4.0f);

    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    // drawing blocks in background
    for (int i = 0; i < 15; ++i) {
        blocks[i].x += blocks[i].speedX * dt;

        if (blocks[i].x - blocks[i].size > 900.0f) {
            blocks[i].size = 18.0f + (rand() % 45);
            blocks[i].x = -900.0f - (rand() % 500);
            blocks[i].y = (rand() % 900) - 450.0f;
            blocks[i].speedX = 450.0f + (rand() % 200);
        }

        float s = blocks[i].size;

        AEGfxSetColorToMultiply(0.15f, 0.9f, 0.15f, 0.45f);
        AEGfxSetColorToAdd(0, 0, 0, 0);
        DrawSprite(mesh, blocks[i].x, blocks[i].y, s, s, 0.0f);
    }

    float tw, th;
    AEGfxGetPrintSize(font, "Flag Protocol", pulseScale, &tw, &th);

    // title centering
    float titleX = -tw / 2.0f;
    float titleY = 0.65f;

    // title
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
        if (IsButtonClicked(playBtn))
            return STATE_NAVIGATION;

        if (IsButtonClicked(controlsBtn))
            return STATE_CONTROLS;

        if (IsButtonClicked(creditBtn))
            return STATE_CREDITS;

        if (IsButtonClicked(editorBtn))
            return STATE_EDITOR;

        if (IsButtonClicked(settingsBtn))
            return STATE_SETTINGS;

        if (IsButtonClicked(quitBtn)) {
            confirmAction = CONFIRM_QUIT;
        }
    }

    static bool showScoreboard = false;

    // Scoreboard button - top right
    Button scoreboardBtn = { 650.0f, 380.0f, 160.0f, 45.0f, "SCOREBOARD" };
    DrawButton(scoreboardBtn, mesh, font);
    if (IsButtonClicked(scoreboardBtn))
        showScoreboard = !showScoreboard;

    if (showScoreboard) {
        float popX = 650.0f;  // world space center X
        float popY = 150.0f;  // world space center Y

        // Background box
        AEGfxSetTransparency(0.85f);
        AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
        AEGfxSetColorToAdd(0, 0, 0, 0);
        DrawSprite(mesh, popX, popY, 200.0f, 320.0f, 0.0f);
        AEGfxSetTransparency(1.0f);

        float popCenterNX = popX / (SCREEN_WIDTH / 2.0f);

        // Title
        const char* topStr = "TOP SCORES:";
        AEGfxGetPrintSize(font, topStr, 0.9f, &tw, &th);
        //AEGfxPrint(paramets) // font, string, x, y, scale, r, g, b, a
        AEGfxPrint(font, const_cast<char*>(topStr), popCenterNX - tw / 2, (popY + 100.0f) / (SCREEN_HEIGHT / 2.0f), 0.9f, 1, 0.8f, 0, 1);
        for (int i = 0; i < 3; i++) {
            char entry[64];
            sprintf_s(entry, "%d. %s  %d", i + 1, highScores[i].name, highScores[i].score);
            AEGfxGetPrintSize(font, entry, 0.8f, &tw, &th);
            AEGfxPrint(font, entry, popCenterNX - tw / 2,
                (popY + 50.0f - i * 60.0f) / (SCREEN_HEIGHT / 2.0f),
                0.8f, 1, 1, 1, 1);
        }
    }

        if (confirmAction == CONFIRM_QUIT) {
            Button yesBtn = { -110.0f, -55.0f, 140.0f, 45.0f, "YES" };
            Button noBtn = { 110.0f, -55.0f, 140.0f, 45.0f, "NO" };

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

    // Tracks destructive actions that require confirmation
    static ConfirmAction confirmAction = CONFIRM_NONE;

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
    DrawControlsHint(fontID);

    // Only allow instant resume when no confirmation popup is active
    if (confirmAction == CONFIRM_NONE) {
        // Using P key to resume from Paused screen
        if (AEInputCheckTriggered(AEVK_P) || AEInputCheckTriggered(AEVK_ESCAPE)) {
            return previous;
        }

        // Button interactions
        if (IsButtonClicked(resumeBtn)) {
            return previous;
        }

        if (IsButtonClicked(menuBtn)) {
            confirmAction = CONFIRM_MENU;
        }

        if (IsButtonClicked(quitBtn)) {
            confirmAction = CONFIRM_QUIT;
        }
    }

    if (confirmAction != CONFIRM_NONE) {
        Button yesBtn = { -110.0f, -55.0f, 140.0f, 45.0f, "YES" };
        Button noBtn = { 110.0f, -55.0f, 140.0f, 45.0f, "NO" };

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

            if (IsButtonClicked(noBtn) || AEInputCheckTriggered(AEVK_ESCAPE)) {
                confirmAction = CONFIRM_NONE;
            }
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

            if (IsButtonClicked(noBtn) || AEInputCheckTriggered(AEVK_ESCAPE)) {
                confirmAction = CONFIRM_NONE;
            }
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
    GetNormalizedTextPosition(-620.0f, -60.0f, &xKey, &y);
    AEGfxPrint(font, "R", xKey, y, 1.0f, 1, 1, 1, 1);
    AEGfxPrint(font, "- Reload", xDesc, y, 1.0f, 1, 1, 1, 1);

    // Switch weapons
    GetNormalizedTextPosition(-620.0f, -100.0f, &xKey, &y);
    AEGfxPrint(font, "1 / 2 / 3 or MouseScroll", xKey, y, 1.0f, 1, 1, 1, 1);
    AEGfxPrint(font, "- Switch Weapons", xDesc, y, 1.0f, 1, 1, 1, 1);

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
    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    float tw, th;

    // Title
    AEGfxGetPrintSize(font, "YOU WIN!", 2.5f, &tw, &th);
    AEGfxPrint(font, const_cast<char*>("YOU WIN!"), -tw / 2, 0.75f, 2.5f, 0.0f, 1.0f, 0.0f, 1.0f);

    // Stats
    char scoreStr[64], gruntsStr[64], flagStr[64];
    sprintf_s(scoreStr, "SCORE: %d", player.score);
    sprintf_s(gruntsStr, "VIRUSES KILLED: %d", player.gruntsKilled);
    sprintf_s(flagStr, "FLAGS CAPTURED: %d", player.flagCaptures);

    AEGfxGetPrintSize(font, scoreStr, 1.2f, &tw, &th);
    AEGfxPrint(font, scoreStr, -tw / 2, 0.35f, 1.2f, 1, 1, 1, 1);
    AEGfxGetPrintSize(font, gruntsStr, 1.2f, &tw, &th);
    AEGfxPrint(font, gruntsStr, -tw / 2, 0.15f, 1.2f, 1, 1, 1, 1);
    AEGfxGetPrintSize(font, flagStr, 1.2f, &tw, &th);
    AEGfxPrint(font, flagStr, -tw / 2, -0.05f, 1.2f, 1, 1, 1, 1);

    bool fromMainGame = (previous == STATE_GAME);

    if (fromMainGame) {
        if (IsTopThree(player.score) && !scoreSaved) {
            // Name entry prompt
            const char* promptStr = "TOP 3 SCORE! ENTER YOUR NAME:";
            AEGfxGetPrintSize(font, promptStr, 1.0f, &tw, &th);
            AEGfxPrint(font, const_cast<char*>(promptStr), -tw / 2, -0.22f, 1.0f, 1, 0.8f, 0, 1);

            // Keyboard input
            for (u8 k = AEVK_A; k <= AEVK_Z; k++) {
                if (AEInputCheckTriggered(k) && nameInputLen < 12) {
                    nameInputBuffer[nameInputLen++] = 'A' + (k - AEVK_A);
                    nameInputBuffer[nameInputLen] = '\0';
                }
            }
            if (AEInputCheckTriggered(AEVK_BACK) && nameInputLen > 0)
                nameInputBuffer[--nameInputLen] = '\0';

            char displayName[64];
            sprintf_s(displayName, "> %s_", nameInputBuffer);
            AEGfxGetPrintSize(font, displayName, 1.2f, &tw, &th);
            AEGfxPrint(font, displayName, -tw / 2, -0.36f, 1.2f, 0, 1, 1, 1);

            if (nameInputLen > 0) {
                Button saveBtn = { 0.0f, -290.0f, 200.0f, 50.0f, "SAVE SCORE" };
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
            // Leaderboard
            const char* topStr = "TOP SCORES:";
            AEGfxGetPrintSize(font, topStr, 1.0f, &tw, &th);
            AEGfxPrint(font, const_cast<char*>(topStr), -tw / 2, -0.22f, 1.0f, 1, 0.8f, 0, 1);

            for (int i = 0; i < 3; i++) {
                char entry[64];
                sprintf_s(entry, "%d. %s  %d", i + 1, highScores[i].name, highScores[i].score);
                AEGfxGetPrintSize(font, entry, 0.9f, &tw, &th);
                AEGfxPrint(font, entry, -tw / 2, -0.34f - i * 0.12f, 0.9f, 1, 1, 1, 1);
            }
        }
    }

    // Nav buttons — lower if scoreboard shown
    float btnY = fromMainGame ? -390.0f : -250.0f;
    Button menuBtn = { -150.0f, btnY, 200.0f, 50.0f, "MAIN MENU" };
    Button creditsBtn = { 150.0f, btnY, 200.0f, 50.0f, "CREDITS" };
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

    AEGfxSetBackgroundColor(0.1f, 0.05f, 0.05f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    float tw, th;

    // Title
    AEGfxGetPrintSize(font, "YOU DIED!", 2.5f, &tw, &th);
    AEGfxPrint(font, const_cast<char*>("YOU DIED!"), -tw / 2, 0.75f, 2.5f, 1.0f, 0.0f, 0.0f, 1.0f);

    // Stats
    char scoreStr[64], gruntsStr[64], flagStr[64];
    sprintf_s(scoreStr, "SCORE: %d", player.score);
    sprintf_s(gruntsStr, "VIRUSES KILLED: %d", player.gruntsKilled);
    sprintf_s(flagStr, "FLAGS CAPTURED: %d", player.flagCaptures);

    AEGfxGetPrintSize(font, scoreStr, 1.2f, &tw, &th);
    AEGfxPrint(font, scoreStr, -tw / 2, 0.35f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f);

    AEGfxGetPrintSize(font, gruntsStr, 1.2f, &tw, &th);
    AEGfxPrint(font, gruntsStr, -tw / 2, 0.15f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f);

    AEGfxGetPrintSize(font, flagStr, 1.2f, &tw, &th);
    AEGfxPrint(font, flagStr, -tw / 2, -0.05f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f);

    // Buttons
    Button retryBtn = { 0.0f, -150.0f, 200.0f, 50.0f, "RETRY" };
    Button menuBtn = { 0.0f, -250.0f, 200.0f, 50.0f, "MAIN MENU" };

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
    const int totalPages = 4;

    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    float tw, th;

    // Title
    AEGfxGetPrintSize(font, "Credits", 2.2f, &tw, &th);
    AEGfxPrint(font, const_cast<char*>("Credits"), -tw / 2.0f, 0.78f, 2.2f, 1.0f, 0.92f, 0.92f, 1.0f);

    // Page content
    switch (creditsPage) {
    case 0:
    {
        const char* line1 = "WWW.DIGIPEN.EDU";
        const char* line2 = "All content (C) 2026 DigiPen Institute of Technology Singapore";
        const char* line3 = "All Rights Reserved.";

        AEGfxGetPrintSize(font, line1, 1.1f, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(line1), -tw / 2.0f, 0.12f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f);

        AEGfxGetPrintSize(font, line2, 1.05f, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(line2), -tw / 2.0f, 0.00f, 1.05f, 1.0f, 1.0f, 1.0f, 1.0f);

        AEGfxGetPrintSize(font, line3, 1.05f, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(line3), -tw / 2.0f, -0.08f, 1.05f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    break;

    case 1:
    {
        const char* header = "TEAM MEMBERS";
        const char* names[] = {
            "Yeo Kah Boon Joshua",
            "Ye Min Thwin",
            "Zhang Yida",
            "Pang Jun Yuan"
        };

        AEGfxGetPrintSize(font, header, 1.3f, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(header), -tw / 2.0f, 0.32f, 1.3f, 0.0f, 1.0f, 0.8f, 1.0f);

        float startY = 0.18f;
        for (int i = 0; i < 4; ++i) {
            AEGfxGetPrintSize(font, names[i], 1.1f, &tw, &th);
            AEGfxPrint(font, const_cast<char*>(names[i]), -tw / 2.0f, startY - i * 0.13f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
    break;

    case 2:
    {
        const char* header = "FACULTY & INSTRUCTORS";
        const char* presidentHeader = "PRESIDENT";
        const char* presidentName = "Claude Comair";
        const char* instructorHeader = "INSTRUCTORS";
        const char* instructors[] = {
            "Gerald Wong Han Feng",
            "Dr. Soroor Malekmohammadai Faradounbeh",
            "Tommy Tan Chee Wei"
        };

        AEGfxGetPrintSize(font, header, 1.3f, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(header), -tw / 2.0f, 0.36f, 1.3f, 0.0f, 1.0f, 0.8f, 1.0f);

        AEGfxGetPrintSize(font, presidentHeader, 1.05f, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(presidentHeader), -tw / 2.0f, 0.22f, 1.05f, 1.0f, 0.9f, 0.3f, 1.0f);

        AEGfxGetPrintSize(font, presidentName, 1.05f, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(presidentName), -tw / 2.0f, 0.14f, 1.05f, 1.0f, 1.0f, 1.0f, 1.0f);

        AEGfxGetPrintSize(font, instructorHeader, 1.05f, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(instructorHeader), -tw / 2.0f, 0.01f, 1.05f, 1.0f, 0.9f, 0.3f, 1.0f);

        float startY = -0.07f;
        for (int i = 0; i < 3; ++i) {
            AEGfxGetPrintSize(font, instructors[i], 1.0f, &tw, &th);
            AEGfxPrint(font, const_cast<char*>(instructors[i]), -tw / 2.0f, startY - i * 0.12f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
    break;

    case 3:
    {
        const char* header = "SPECIAL THANKS ;)";
        const char* thanks[] = {
            "Our instructors",
            "Our classmates",
            "Our playtesters"
        };

        AEGfxGetPrintSize(font, header, 1.3f, &tw, &th);
        AEGfxPrint(font, const_cast<char*>(header), -tw / 2.0f, 0.28f, 1.3f, 0.0f, 1.0f, 0.8f, 1.0f);

        float startY = 0.14f;
        for (int i = 0; i < 3; ++i) {
            AEGfxGetPrintSize(font, thanks[i], 1.1f, &tw, &th);
            AEGfxPrint(font, const_cast<char*>(thanks[i]), -tw / 2.0f, startY - i * 0.13f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
    break;
    }

    // Page indicator
    char pageText[32];
    sprintf_s(pageText, "Page %d / %d", creditsPage + 1, totalPages);
    AEGfxGetPrintSize(font, pageText, 1.1f, &tw, &th);
    AEGfxPrint(font, pageText, -tw / 2.0f, -0.62f, 1.1f, 1.0f, 0.92f, 0.92f, 1.0f);

    // Buttons
    Button prevBtn = { -520.0f, -360.0f, 180.0f, 60.0f, "<<" };
    Button backBtn = { 0.0f, -360.0f, 260.0f, 60.0f, "Back" };
    Button nextBtn = { 520.0f, -360.0f, 180.0f, 60.0f, ">>" };

    DrawButton(prevBtn, mesh, font);
    DrawButton(backBtn, mesh, font);
    DrawButton(nextBtn, mesh, font);

    if ((IsButtonClicked(prevBtn) || AEInputCheckTriggered(AEVK_LEFT)) && creditsPage > 0) {
        creditsPage--;
    }

    if ((IsButtonClicked(nextBtn) || AEInputCheckTriggered(AEVK_RIGHT)) && creditsPage < totalPages - 1) {
        creditsPage++;
    }

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
        printf("Failed to load map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        printf("Map loaded successfully!\n");
    }
    Spawn_Flag(gameMap);
    floodfill(gameMap, 0, 0);
}

void GameStateLoadTutorial(int& gGameRunning, std::vector<std::vector<int>>& gameMap) {
    if (!Load_Game_Level("Assets/Tutorial.txt", gameMap)) {
        printf("Failed to load tutorial map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        printf("Tutorial map loaded successfully!\n");
    }
    Spawn_Flag(gameMap);
}

void GameStateLoadCustom(int& gGameRunning, std::vector<std::vector<int>>& gameMap) {
    if (!Load_Game_Level("Assets/My_Custom_Map.txt", gameMap)) {
        printf("Failed to load custom map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        printf("Custom map loaded successfully!\n");
    }
    Spawn_Flag(gameMap);
}
void GameStateLoadEditor(int& gGameRunning, std::vector<std::vector<int>>& gameMap) {
    static bool loaded = false;
    if (loaded) return;

    if (!Load_Game_Level("Assets/My_Custom_Map.txt", gameMap)) {
        printf("Failed to load editor map! Exiting...\n");
        gGameRunning = 0;
    }
    else {
        printf("Editor map loaded successfully!\n");
    }
    loaded = true;
}

void LoadHighScores() {
    FILE* f;
    fopen_s(&f, "Assets/highscores.txt", "r");
    if (!f) return;
    for (int i = 0; i < 3; i++) {
        if (fscanf_s(f, "%31s %d", highScores[i].name, 32, &highScores[i].score) != 2) {
            printf("Loaded score %d: %s %d\n", i, highScores[i].name, highScores[i].score);


            break;
        }
    }
    fclose(f);
}

void SaveHighScores() {
    
    FILE* f;
    fopen_s(&f, "Assets/highscores.txt", "w");
    if (!f) return;
    for (int i = 0; i < 3; i++)
        fprintf(f, "%s %d\n", highScores[i].name, highScores[i].score);
    fclose(f);
}

bool IsTopThree(int score) {
    return score > highScores[2].score;
}

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