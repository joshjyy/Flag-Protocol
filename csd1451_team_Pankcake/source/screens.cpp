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
    AEGfxPrint(font, const_cast<char*>(btn.label), nx, ny, textScale, 0.0f, 1.0f, 0.0f, 1.0f);
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

