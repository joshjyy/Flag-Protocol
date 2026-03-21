#pragma once
#include "AEEngine.h"
#include "player.hpp"

// ---------------------------------------------------------------------------
// Event Manager
// ---------------------------------------------------------------------------
struct EventManager {
    bool winTriggered;
	bool playerDied;
};

EventManager InitEventManager(void);
void CheckWinCondition(EventManager& events, Player& player);

// ---------------------------------------------------------------------------
// Cutscene constants
// ---------------------------------------------------------------------------
const float CUT_FADE_IN_DUR = 2.5f;
const float CUT_HOLD_DUR = 20.0f;
const float CUT_FADE_OUT_DUR = 1.5f;
const int   CUT_DOT_COUNT = 800;
const float CUT_DOT_SIZE = 4.0f;
const float CUT_TITLE_SCALE = 1.4f;
const float CUT_TEXT_SCALE = 0.9f;
const float CUT_TEXT_START_Y = 200.0f;
const float CUT_TEXT_STEP_Y = 55.0f;
const float CUT_TEXT_X = -300.0f;

// ---------------------------------------------------------------------------
// Cutscene
// ---------------------------------------------------------------------------
enum CutscenePhase {
    CUT_FADEIN = 0,
    CUT_HOLD = 1,
    CUT_FADEOUT = 2,
    CUT_DONE = 3
};

struct CutsceneData {
    CutscenePhase phase = CUT_FADEIN;
    float         timer = 0.0f;
    float         alpha = 1.0f;
};

bool Cutscene_Update(CutsceneData& cut, float dt);
void Cutscene_Draw(const CutsceneData& cut, s8 font, AEGfxVertexList* squareMesh);