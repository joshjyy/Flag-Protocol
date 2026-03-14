#pragma once
#include "AEEngine.h"
#include "player.hpp"
#include "utils.hpp"
#include <vector>

void Tutorial_RenderHints(s8 font)
{
    struct Hint {
        const char* text;
        float r, g, b;
    };

    Hint hints[] = {
        { "G: God Mode",       0.4f, 1.0f, 0.4f },
        { "V: Unlimited Ammo", 0.4f, 0.8f, 1.0f },
        { "K: Die / Respawn",            1.0f, 0.4f, 0.4f },
        { "P: Drop Flag",      1.0f, 1.0f, 0.4f },
        { "Step on Lime Tile to Obtain Flag Node!",          0.0f, 1.0f, 1.0f },  
        { "Step on Purple Tile to Activate Flag Node!",      0.5f, 0.0f, 1.0f },


    };

    float startY = 390.0f;
    float stepY = 30.0f;
    float xPos = -780.0f;

    float textX, textY;
    for (int i = 0; i < 4; i++) {
        GetNormalizedTextPosition(xPos, startY - i * stepY, &textX, &textY);
        AEGfxPrint(fontID, hints[i].text, textX, textY, 0.8f,
            hints[i].r, hints[i].g, hints[i].b, 1.0f);
    }
    // index 4 =
    GetNormalizedTextPosition(xPos, startY - 4 * stepY - 20.0f, &textX, &textY);
    AEGfxPrint(font, hints[4].text, textX, textY, 0.7f,
        hints[4].r, hints[4].g, hints[4].b, 1.0f);

    // index 5 
    GetNormalizedTextPosition(xPos, startY - 5 * stepY - 20.0f, &textX, &textY);
    AEGfxPrint(font, hints[5].text, textX, textY, 0.7f,
        hints[5].r, hints[5].g, hints[5].b, 1.0f);
}

void Range_Hacks(Player& player)
{
    // God Mode — lock health to max
    static bool godMode = false;
    if (AEInputCheckTriggered(AEVK_G)) godMode = !godMode;
    if (godMode) player.health = 100.0f;

    // Unlimited ammo — refill both weapons every frame
    static bool unlimitedAmmo = false;
    if (AEInputCheckTriggered(AEVK_V)) unlimitedAmmo = !unlimitedAmmo;
    if (unlimitedAmmo) {
        player.pistol.currentAmmo = player.pistol.maxAmmo;
        player.rifle.currentAmmo = player.rifle.maxAmmo;
    }

    // Kill player
    if (AEInputCheckTriggered(AEVK_K)) player.health = 0.0f;

    // Drop flag
    if (AEInputCheckTriggered(AEVK_P)) player.holdingFlag = false;
}