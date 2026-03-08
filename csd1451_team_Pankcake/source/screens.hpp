#pragma once
#include "AEEngine.h"

//Game state enums for managing different screens
enum GameState { STATE_MENU, STATE_GAME, STATE_EDITOR , STATE_TUTORIAL };

//button struct
struct Button { float x, y, w, h; const char* label; };

bool IsButtonClicked(const Button& btn);
void DrawButton(const Button& btn, AEGfxVertexList* mesh, s8 font);
GameState UpdateAndRenderMenu(AEGfxVertexList* mesh, s8 font, float dt);
bool IsQuitClicked();