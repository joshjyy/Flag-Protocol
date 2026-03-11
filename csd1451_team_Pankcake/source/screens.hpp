#pragma once
#include "AEEngine.h"

//Game state enums for managing different screens
<<<<<<< Updated upstream
enum GameState { STATE_MENU, STATE_GAME, STATE_EDITOR , STATE_TUTORIAL, STATE_QUIT };
=======
enum GameState { STATE_MENU, STATE_GAME, STATE_EDITOR, STATE_LOAD, STATE_TUTORIAL, STATE_QUIT, STATE_PAUSE };
>>>>>>> Stashed changes

//button struct
struct Button { float x, y, w, h; const char* label; };

bool IsButtonClicked(const Button& btn);
void DrawButton(const Button& btn, AEGfxVertexList* mesh, s8 font);
GameState UpdateAndRenderMenu(AEGfxVertexList* mesh, s8 font, float dt);
<<<<<<< Updated upstream
=======
GameState UpdateAndRenderPause(AEGfxVertexList* mesh, s8 font, float dt);

void CurrGamestate(GameState state, int& gGameRunning, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
void GameStateLoad(int& gGameRunning, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
void GameStateLoad(int& gGameRunning, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
void GameStateLoadTutorial(int& gGameRunning, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
void GameStateLoadCustom(int& gGameRunning, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
>>>>>>> Stashed changes
