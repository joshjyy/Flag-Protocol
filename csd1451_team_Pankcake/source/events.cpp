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
        events.winTriggered = true;
        printf("=== YOU WIN! ===\n");
    }

    if (player.isDead && player.deathExplosionSpawned) {
        bool allDead = true;
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (player.deathEmitter->particles[i].active) {
                allDead = false;
                break;
            }
        }
        if (allDead) events.playerDied = true;
    }


  //  if (events.winTriggered) {
		//events.winTriggered = false; // Reset for potential future use
  //      float textX, textY;
  //      GetNormalizedTextPosition(-100.0f, -150.0f, &textX, &textY);
  //      AEGfxPrint(fontID, "YOU WIN!", textX, textY, 3.0f, 0.0f, 1.0f, 0.0f, 1.0f);

  //      char scoreText[64];
  //      sprintf_s(scoreText, 64, "Grunts Killed: %d", player.gruntsKilled);
  //      GetNormalizedTextPosition(-75.0f, -200.0f, &textX, &textY);
  //      AEGfxPrint(fontID, scoreText, textX, textY, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f);
  //  }
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
        if (cut.timer >= CUT_HOLD_DUR || AEInputCheckTriggered(AEVK_RETURN)) {
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
        "MISSION BRIEFING",
        "Obtain 10 Flag Nodes to Clear.",
        "Your health depletes every second.",
        "Recover health by retrieving flags and killing enemies.",
        "Stay alive. Score as high as you can.",
        "",
        "[ ENTER to skip ]"
    };
    float colours[][3] = {
        { 1.0f, 0.8f, 0.2f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 0.4f, 0.4f },
        { 0.4f, 1.0f, 0.4f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 0.5f, 0.5f, 0.5f },
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