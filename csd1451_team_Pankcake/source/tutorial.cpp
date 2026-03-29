/*!************************************************************************
  \file tutorial.cpp
  \author
  Yeo Kah Boon Joshua (Tutorial UI for Player)





  \brief
  This source file contains definitions of the stuff related to the tutorial level.

**************************************************************************/

#include "gameMode.hpp"
#include "utils.hpp"
#include "enemies.hpp"
#include "player.hpp"
#include "damageNumbers.hpp"
#include "gameConstants.hpp"
#include "AI.hpp"
#include "weapon.hpp"
#include "events.hpp"
#include "map.hpp"
#include "tutorial.hpp"
#include <vector>

void Tutorial_RenderHints(s8 font)
{
    struct Hint {
        const char* text;
        float r, g, b;
    };

    Hint hints[] = {
        { "G: Admin Mode",       0.4f, 1.0f, 0.4f },
        { "V: Unlimited Ammo", 0.4f, 0.8f, 1.0f },
        { "K: Die / Respawn",            1.0f, 0.4f, 0.4f },
        { "M: Drop Flag",      1.0f, 1.0f, 0.4f },
        { "Look for the Lime Tile and Step On It to Obtain Flag Node!",          0.0f, 1.0f, 1.0f },
        { "Look for the Purple Tile and Step On It To Connect Flag Node for Points!",      0.5f, 0.0f, 1.0f },
    };

    float startY = TUTORIAL_HINT_START_Y;
    float stepY = TUTORIAL_HINT_STEP_Y;
    float xPos = TUTORIAL_HINT_X;

    float textX, textY;
    for (int i = 0; i < TUTORIAL_HINT_KEYBIND_COUNT; i++) {
        GetNormalizedTextPosition(xPos, startY - i * stepY, &textX, &textY);
        AEGfxPrint(fontID, hints[i].text, textX, textY, TUTORIAL_HINT_SCALE,
            hints[i].r, hints[i].g, hints[i].b, 1.0f);
    }
    // index 4
    GetNormalizedTextPosition(xPos, startY - 4 * stepY - TUTORIAL_HINT_EXTRA_OFFSET, &textX, &textY);
    AEGfxPrint(font, hints[4].text, textX, textY, TUTORIAL_HINT_SCALE_SMALL,
        hints[4].r, hints[4].g, hints[4].b, 1.0f);

    // index 5
    GetNormalizedTextPosition(xPos, startY - 5 * stepY - TUTORIAL_HINT_EXTRA_OFFSET, &textX, &textY);
    AEGfxPrint(font, hints[5].text, textX, textY, TUTORIAL_HINT_SCALE_SMALL,
        hints[5].r, hints[5].g, hints[5].b, 1.0f);

    char fpsText[32];
    sprintf_s(fpsText, 32, "FPS: %.0f | DT: %.4f",
        AEFrameRateControllerGetFrameRate(),
        AEFrameRateControllerGetFrameTime());

    GetNormalizedTextPosition(xPos, startY - 6 * stepY - TUTORIAL_HINT_EXTRA_OFFSET, &textX, &textY);
    AEGfxPrint(font, fpsText, textX, textY, TUTORIAL_HINT_SCALE_SMALL, 1.0f, 1.0f, 1.0f, 1.0f);
}

void Range_Hacks(Player& player)
{
    // God Mode — lock health to max
    static bool godMode = false;
    if (AEInputCheckTriggered(AEVK_G)) godMode = !godMode;
    if (godMode) player.health = PLAYER_MAX_HEALTH;

    // Unlimited ammo — refill both weapons every frame
    static bool unlimitedAmmo = false;
    if (AEInputCheckTriggered(AEVK_V)) unlimitedAmmo = !unlimitedAmmo;
    if (unlimitedAmmo) {
        player.pistol.currentAmmo = player.pistol.maxAmmo;
        player.rifle.currentAmmo = player.rifle.maxAmmo;
        player.shotgun.currentAmmo = player.shotgun.maxAmmo;
    }

    // Kill player
    if (AEInputCheckTriggered(AEVK_K)) player.health = 0.0f;

    // Drop flag
    if (AEInputCheckTriggered(AEVK_M)) player.holdingFlag = false;
}

void Tutorial_RenderDebug(s8 font, const Player& player)
{
    char buf[64];
    float textX, textY;
    float xPos = TUTORIAL_DEBUG_X;
    float startY = TUTORIAL_DEBUG_START_Y;
    float stepY = TUTORIAL_DEBUG_STEP_Y;
    int   i = 0;

    // Position
    sprintf_s(buf, 64, "X: %.1f  Y: %.1f", player.x, player.y);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, TUTORIAL_HINT_SCALE_SMALL, 1.0f, 1.0f, 1.0f, 1.0f);

    // Health
    sprintf_s(buf, 64, "HP: %.1f", player.health);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, TUTORIAL_HINT_SCALE_SMALL, 1.0f, 0.4f, 0.4f, 1.0f);

    // Ammo
    sprintf_s(buf, 64, "Pistol: %d/%d", player.pistol.currentAmmo, player.pistol.maxAmmo);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, TUTORIAL_HINT_SCALE_SMALL, 0.8f, 0.8f, 1.0f, 1.0f);

    sprintf_s(buf, 64, "Rifle:  %d/%d", player.rifle.currentAmmo, player.rifle.maxAmmo);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, TUTORIAL_HINT_SCALE_SMALL, 0.8f, 0.8f, 1.0f, 1.0f);

    sprintf_s(buf, 64, "Shotgun:%d/%d", player.shotgun.currentAmmo, player.shotgun.maxAmmo);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, TUTORIAL_HINT_SCALE_SMALL, 0.8f, 0.8f, 1.0f, 1.0f);

    // Velocity
    sprintf_s(buf, 64, "VX: %.2f  VY: %.2f", player.velocityX, player.velocityY);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, TUTORIAL_HINT_SCALE_SMALL, 1.0f, 0.8f, 0.4f, 1.0f);

    // Dash
    sprintf_s(buf, 64, "Dash CD: %.2f  InDash: %s", player.dashCooldownTimer, player.inDash ? "YES" : "NO");
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, TUTORIAL_HINT_SCALE_SMALL, 0.4f, 1.0f, 1.0f, 1.0f);

    // Flag + score
    sprintf_s(buf, 64, "Flag: %s  Score: %d", player.holdingFlag ? "HELD" : "NONE", player.score);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, TUTORIAL_HINT_SCALE_SMALL, 1.0f, 1.0f, 0.4f, 1.0f);

    // Kills
    sprintf_s(buf, 64, "Kills: %d", player.gruntsKilled);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, TUTORIAL_HINT_SCALE_SMALL, 1.0f, 0.4f, 1.0f, 1.0f);
}

void UpdateAndRenderTutorial(
    Player& player,
    float dt,
    std::vector<std::vector<int>>* gameMap,
    CameraPos& camera,
    std::vector<Bullet>& playerBullets,
    std::vector<Bullet>& enemyBullets,
    std::vector<Grunt>& grunts,
    EventManager& events)
{
    (void)events;  // Unused in this mode

    WeaponStats weapon = GetCurrentWeapon(player);

    // Update player
    UpdatePlayer(&player, dt, *gameMap);
    WeaponSwitchCheck(player);
    if (player.currentWeaponIndex == 2)
        SpreadShootingCheck(player, playerBullets, weapon, camera, SHOTGUN_PELLET_COUNT, SHOTGUN_SPREAD_ANGLE);
    else
        ShootingCheck(player, playerBullets, weapon, camera);
    ReloadBullets(weapon, dt);
        
    UpdateBullets(playerBullets, dt, *gameMap);

    // Update enemies
    UpdateGruntSpawner(grunts, *gameMap, dt);
    UpdateGrunts(grunts, player, enemyBullets, dt, *gameMap);
    UpdateBullets(enemyBullets, dt, *gameMap);
    CheckBulletGruntCollisions(playerBullets, grunts, player);
    RemoveDeadGrunts(grunts);
    CheckPlayerGruntBodyCollision(player, grunts);
    UpdateDamageNumbers(dt);

    // Render
    DrawPlayer(player, meshes.circle,meshes.arrow, camera, dt, *gameMap);
    RenderGrunts(grunts, meshes.circle, camera);
    RenderBullets(player, playerBullets, meshes.circle, camera);
    RenderBullets(enemyBullets, meshes.circle, camera);
    DrawPlayerUI(player, weapon, meshes.square, meshes.circle, camera);
    DrawCrosshair(meshes.square);
    RenderDamageNumbers(camera);
}