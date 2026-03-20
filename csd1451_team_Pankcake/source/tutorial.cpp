#include "gameMode.hpp"
#include "utils.hpp"
#include "enemies.hpp"
#include "player.hpp"
#include "damageNumbers.hpp"
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
        { "G: God Mode",       0.4f, 1.0f, 0.4f },
        { "V: Unlimited Ammo", 0.4f, 0.8f, 1.0f },
        { "K: Die / Respawn",            1.0f, 0.4f, 0.4f },
        { "P: Drop Flag",      1.0f, 1.0f, 0.4f },
        { "Look for the Lime Tile and Step On It to Obtain Flag Node!",          0.0f, 1.0f, 1.0f },  
        { "Look for the Purple Tile and Step On It To Connect Flag Node for Points!",      0.5f, 0.0f, 1.0f },


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

    char fpsText[32];
    sprintf_s(fpsText, 32, "FPS: %.0f | DT: %.4f",
        AEFrameRateControllerGetFrameRate(),
        AEFrameRateControllerGetFrameTime());

    // Move FPS higher so it's on screen
    GetNormalizedTextPosition(xPos, startY - 6 * stepY - 20.0f, &textX, &textY);
    AEGfxPrint(font, fpsText, textX, textY, 0.7f, 1.0f, 1.0f, 1.0f, 1.0f);
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

void Tutorial_RenderDebug(s8 font, const Player& player)
{
    char buf[64];
    float textX, textY;
    float xPos = 600.0f;   // right side
    float startY = 390.0f;
    float stepY = 28.0f;
    int   i = 0;

    // Position
    sprintf_s(buf, 64, "X: %.1f  Y: %.1f", player.x, player.y);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, 0.7f, 1.0f, 1.0f, 1.0f, 1.0f);

    // Health
    sprintf_s(buf, 64, "HP: %.1f", player.health);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, 0.7f, 1.0f, 0.4f, 0.4f, 1.0f);

    // Ammo
    sprintf_s(buf, 64, "Pistol: %d/%d", player.pistol.currentAmmo, player.pistol.maxAmmo);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, 0.7f, 0.8f, 0.8f, 1.0f, 1.0f);

    sprintf_s(buf, 64, "Rifle:  %d/%d", player.rifle.currentAmmo, player.rifle.maxAmmo);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, 0.7f, 0.8f, 0.8f, 1.0f, 1.0f);

    // Velocity
    sprintf_s(buf, 64, "VX: %.2f  VY: %.2f", player.velocityX, player.velocityY);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, 0.7f, 1.0f, 0.8f, 0.4f, 1.0f);

    // Dash
    sprintf_s(buf, 64, "Dash CD: %.2f  InDash: %s", player.dashCooldownTimer, player.inDash ? "YES" : "NO");
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, 0.7f, 0.4f, 1.0f, 1.0f, 1.0f);

    // Flag + score
    sprintf_s(buf, 64, "Flag: %s  Score: %d", player.holdingFlag ? "HELD" : "NONE", player.score);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, 0.7f, 1.0f, 1.0f, 0.4f, 1.0f);

    // Kills
    sprintf_s(buf, 64, "Kills: %d", player.gruntsKilled);
    GetNormalizedTextPosition(xPos, startY - i++ * stepY, &textX, &textY);
    AEGfxPrint(font, buf, textX, textY, 0.7f, 1.0f, 0.4f, 1.0f, 1.0f);
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
	(void)enemyBullets;
	(void)grunts;


    WeaponStats weapon = GetCurrentWeapon(player);

    UpdatePlayer(&player, dt, *gameMap);
    WeaponSwitchCheck(player);
    if (player.currentWeaponIndex == 2)
        SpreadShootingCheck(player, playerBullets, weapon, camera, 5, 30.0f);
    else
        ShootingCheck(player, playerBullets, weapon, camera);  
    ReloadBullets(weapon, dt);

    UpdateBullets(playerBullets, dt, *gameMap);

    DrawPlayer(player, meshes.circle, camera, dt);
    RenderBullets(playerBullets, meshes.circle, camera);
    DrawPlayerUI(player, weapon, meshes.square, meshes.circle, camera);
    DrawCrosshair(meshes.square);

    CheckWinCondition(events, player);
}