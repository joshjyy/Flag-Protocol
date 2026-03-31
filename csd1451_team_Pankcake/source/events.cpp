/*!************************************************************************
  \file events.cpp
  \author
  Yeo Kah Boon Joshua






  \brief
  This source file contains definitions for the game event manager.

**************************************************************************/

#include "events.hpp"
#include "AEEngine.h"
#include "gameConstants.hpp"
#include "utils.hpp"

EventManager InitEventManager(void) {
    EventManager events = {
        false,  // winTriggered
		false   // playerDied

    };
    return events;
}

void CheckWinCondition(EventManager& events, Player& player) {

    if (player.flagCaptures == 10 && !events.winTriggered) {
        ShowCursor(true);

        events.winTriggered = true;
    }

    if (player.isDead && player.deathExplosionSpawned) {
        bool allDead = true;
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (player.deathEmitter->particles[i].active) {
                allDead = false;
                break;
            }
        }
        ShowCursor(true);

        if (allDead) events.playerDied = true;
    }



}

// ---------------------------------------------------------------------------
// Cutscene
// ---------------------------------------------------------------------------
bool Cutscene_Update(CutsceneData& cut, float dt)
{
    cut.timer += dt;

    switch (cut.phase)
    {
    case CUT_FADEIN:
        cut.alpha = 1.0f - (cut.timer / CUT_FADE_IN_DUR);
        if (cut.alpha < 0.0f) cut.alpha = 0.0f;
        if (cut.timer >= CUT_FADE_IN_DUR) {
            cut.phase = CUT_HOLD;
            cut.timer = 0.0f;
            cut.alpha = 0.0f;
        }
        break;

    case CUT_HOLD:
        cut.alpha = 0.0f;
        if (cut.timer >= CUT_HOLD_DUR || AEInputCheckTriggered(AEVK_LBUTTON)) {
            cut.phase = CUT_FADEOUT;
            cut.timer = 0.0f;
        }
        break;

    case CUT_FADEOUT:
        cut.alpha = cut.timer / CUT_FADE_OUT_DUR;
        if (cut.alpha > 1.0f) cut.alpha = 1.0f;
        if (cut.timer >= CUT_FADE_OUT_DUR) {
            cut.phase = CUT_DONE;
            cut.alpha = 1.0f;
        }
        break;

    case CUT_DONE:
        return true;
    }

    return false;
}

void Cutscene_Draw(const CutsceneData& cut, s8 font, AEGfxVertexList* squareMesh)
{
    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.1f);
    float textAlpha = 1.0f - cut.alpha;
    const char* lines[] = {
        "DEBUGGER.EXE",
        "You are a Debugger program injected into a corrupted system.",
        "Rogue processes swarm the network. They will drain your integrity.",
        "Locate FLAG NODES scattered across the system and retrieve them.",
        "Deliver 10 FLAG NODES to the PURPLE ZONES to purge the corruption.",
        "Kill enemies to restore integrity and resupply ammunition.",
        "[ Left-Click to begin ]"
    };
    float colours[][3] = {
        { 0.0f, 1.0f, 0.4f },    // title
        { 0.8f, 0.8f, 1.0f },    // lore
        { 1.0f, 0.4f, 0.4f },    // threat
        { 1.0f, 1.0f, 1.0f },    // objective
        { 1.0f, 1.0f, 1.0f },    // objective
        { 0.4f, 1.0f, 0.4f },    // reward
        { 0.5f, 0.5f, 0.5f },    // skip
    };
    float startY = CUT_TEXT_START_Y;
    float stepY = CUT_TEXT_STEP_Y;
    float textX, textY;
    for (int i = 0; i < 7; i++) {
        GetNormalizedTextPosition(CUT_TEXT_X, startY - i * stepY, &textX, &textY);
        float scale = (i == 0) ? CUT_TITLE_SCALE : CUT_TEXT_SCALE;
        AEGfxPrint(font, lines[i], textX, textY, scale,
            colours[i][0], colours[i][1], colours[i][2], textAlpha);
    }
    // Static noise overlay
    if (cut.alpha > 0.0f)
    {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        int dotCount = (int)(cut.alpha * CUT_DOT_COUNT);
        for (int i = 0; i < dotCount; i++)
        {
            float rx = (float)(rand() % SCREEN_WIDTH) - (SCREEN_WIDTH / 2.0f);
            float ry = (float)(rand() % SCREEN_HEIGHT) - (SCREEN_HEIGHT / 2.0f);
            float brightness = (float)(rand() % 100) / 100.0f;
            AEGfxSetTransparency(cut.alpha);
            AEGfxSetColorToMultiply(0.0f, brightness, 0.0f, cut.alpha);
            AEMtx33 s, t, transform;
            AEMtx33Scale(&s, CUT_DOT_SIZE, CUT_DOT_SIZE);
            AEMtx33Trans(&t, rx, ry);
            AEMtx33Concat(&transform, &t, &s);
            AEGfxSetTransform(transform.m);
            AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);
        }
        AEGfxSetTransparency(1.0f);
    }
}