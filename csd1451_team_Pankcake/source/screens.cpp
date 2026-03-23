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

GameState UpdateAndRenderMenu(AEGfxVertexList* mesh, s8 font, float dt) {
    static float menuAnimTime = 0.0f;
    menuAnimTime += dt;

    static bool initialized = false;

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

    Button playBtn = { 0.0f,  150.0f, 200.0f, 50.0f, "PLAY" };
    Button controlsBtn = { 0.0f,   50.0f, 200.0f, 50.0f, "CONTROLS" };
    Button creditBtn = { 0.0f,  -50.0f, 200.0f, 50.0f, "CREDITS" };
    Button editorBtn = { 0.0f,  -150.0f, 200.0f, 50.0f, "EDITOR" };
    Button quitBtn = { 0.0f, -300.0f, 200.0f, 50.0f, "QUIT" };

    DrawButton(playBtn, mesh, font);
    DrawButton(controlsBtn, mesh, font);
    DrawButton(creditBtn, mesh, font);
    DrawButton(editorBtn, mesh, font);
    DrawButton(quitBtn, mesh, font);

    if (IsButtonClicked(playBtn))   return STATE_NAVIGATION;
    if (IsButtonClicked(controlsBtn)) return STATE_CONTROLS;
    if (IsButtonClicked(creditBtn)) return STATE_CREDITS;
    if (IsButtonClicked(editorBtn)) return STATE_EDITOR;
    if (IsButtonClicked(quitBtn))   return STATE_QUIT;

    return STATE_MENU;
}

GameState UpdateAndRenderPause(AEGfxVertexList* mesh, s8 font, float dt, GameState previous) {
    (void)dt;

  

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

    // Using P key to resume from Paused screen
    if (AEInputCheckTriggered(AEVK_P) || AEInputCheckTriggered(AEVK_ESCAPE)) {
        return previous;
    }

    // Button interactions
    if (IsButtonClicked(resumeBtn)) {
        return previous;
    }
    if (IsButtonClicked(menuBtn)) {
        AEAudioStopGroup(GameBGMGroup);
        AEAudioStopGroup(EditorBGMGroup);
        
        return STATE_MENU;
    }

    if (IsButtonClicked(quitBtn)) {
        return STATE_QUIT;
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
    AEGfxPrint(font, "1 / 2 / 3", xKey, y, 1.0f, 1, 1, 1, 1);
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

GameState UpdateAndRenderWinScreen(AEGfxVertexList* mesh, s8 font, float dt,  Player& player) {
    (void)dt;

    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);


    float tw, th;
    // Title
    AEGfxGetPrintSize(font, "YOU WIN!", 2.5f, &tw, &th);
    AEGfxPrint(font, const_cast<char*>("YOU WIN!"), -tw / 2, 0.75f, 2.5f, 0.0f, 1.0f, 0.0f, 1.0f);

    // Stats - build strings
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
    Button menuBtn = { 0.0f, -150.0f, 200.0f, 50.0f, "MAIN MENU" };
    Button creditsBtn = { 0.0f, -250.0f, 200.0f, 50.0f, "CREDITS" };

    DrawButton(menuBtn, mesh, font);
    DrawButton(creditsBtn, mesh, font);

    if (IsButtonClicked(menuBtn)) {
        AEAudioStopGroup(WinBGMGroup);

        AEAudioStopGroup(GameBGMGroup);

        return STATE_MENU;
    }
    if (IsButtonClicked(creditsBtn)) {
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






void CurrGamestate(GameState state, int& gGameRunning, std::vector<std::vector<int>>& gameMap, Player& player, GameState previousState) {
    if (state == STATE_MENU) {
        LoadPlayer(player);
        while (ShowCursor(true) < 0)
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
            while (ShowCursor(true) < 0)
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
        AEAudioPlay(StaticSound, StaticBGMGroup, 0.7f, 1.0f, -1);
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

