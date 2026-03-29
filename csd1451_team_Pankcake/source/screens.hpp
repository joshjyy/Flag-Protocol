/*!************************************************************************
  \file screens.hpp
  \author 
  Yeo Kah Boon Joshua ( High Score Management, Win/Lose Screens, Navigation Screen )





  \brief
  This source file contains definitions of the stuff related to infographics.

**************************************************************************/


#pragma once
#include "AEEngine.h"
#include <array>
#include "player.hpp"
#include "map.hpp"

//Game state enums for managing different screens
enum GameState { 
    STATE_MENU, 
    STATE_GAME, 
    STATE_CUSTOM, 
    STATE_CONTROLS,  
    STATE_EDITOR, 
    STATE_LOAD, 
    STATE_TUTORIAL, 
    STATE_QUIT, 
    STATE_PAUSE , 
    STATE_CUTSCENE, 
    STATE_NAVIGATION,
    STATE_LOSE, 
    STATE_WIN, 
    STATE_CREDITS, 
    STATE_SETTINGS,
    STATE_DIGIPENLOGO,
    };

// Confirmation state enums for handling destructive actions (back to menu and quit)
enum ConfirmAction
{
    CONFIRM_NONE = 0,
    CONFIRM_MENU,
    CONFIRM_QUIT
};

//button struct
struct Button { float x, y, w, h; const char* label; };

//high score entry struct and related variables for managing high scores
struct HighScoreEntry {
    char name[32];
    int  score;
};

static HighScoreEntry highScores[3] = { {"---", 0}, {"---", 0}, {"---", 0} };
static char  nameInputBuffer[32] = "";
static int   nameInputLen = 0;
static bool  scoreSaved = false;


bool IsButtonClicked(const Button& btn);
bool IsButtonHovered(const Button& btn);
void DrawButton(const Button& btn, AEGfxVertexList* mesh, s8 font);

GameState UpdateAndRenderMenu(AEGfxVertexList* mesh, s8 font, float dt);
GameState UpdateAndRenderPause(AEGfxVertexList* mesh, s8 font, float dt, GameState previous);
GameState UpdateAndRenderNavigation(AEGfxVertexList* mesh, s8 font, float dt);
GameState UpdateAndRenderWinScreen(AEGfxVertexList* mesh, s8 font, float dt, Player& player, GameState previous);
GameState UpdateAndRenderLoseScreen(AEGfxVertexList* mesh, s8 font, float dt, Player& player, GameState previous);
GameState UpdateAndRenderControls(AEGfxVertexList* mesh, s8 font, float dt);
GameState UpdateAndRenderCredits(AEGfxVertexList* mesh, s8 font, float dt);
GameState UpdateAndRenderSplash(AEGfxVertexList* mesh, float dt);


void CurrGamestate(GameState state, int& gGameRunning, std::vector<std::vector<int>>& gameMap, Player& player , GameState previousState);
void GameStateLoad(int& gGameRunning, std::vector<std::vector<int>>& gameMap);
void GameStateLoad(int& gGameRunning, std::vector<std::vector<int>>& gameMap);
void GameStateLoadTutorial(int& gGameRunning, std::vector<std::vector<int>>& gameMap);
void GameStateLoadCustom(int& gGameRunning, std::vector<std::vector<int>>& gameMap);
void GameStateLoadEditor(int& gGameRunning, std::vector<std::vector<int>>& gameMap);


void LoadHighScores();  
void SaveHighScores();
bool IsTopThree(int score);
void InsertHighScore(const char* name, int score);

void LoadSplashAssets();
void FreeSplashAssets();