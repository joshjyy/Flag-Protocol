/*!************************************************************************
  \file gameMode.cpp
  \author
  Yeo Kah Boon Joshua (Player Gameplay )






  \brief
  This source file contains definitions of the stuff related to the main level gameplay.

**************************************************************************/

#include "gameMode.hpp"
#include "utils.hpp"
#include "enemies.hpp"
#include "player.hpp"
#include "damageNumbers.hpp"
#include "AI.hpp"
#include "weapon.hpp"
#include "events.hpp"
#include "map.hpp"
#include "debris.hpp"

void DrawControlsHint(s8 font)
{
    struct HintLine {
        const char* text;
        float r, g, b;
    };

    HintLine hints[] = {
        { "[ WASD ] Move   [ RMB ] Sprint   [ SPACE ] Dash",  1.0f, 0.9f, 0.3f },
        { "[ LMB ] Shoot   [ R ] Reload   [ 1/2/3 ] or [MouseScroll] Weapon",  1.0f, 0.9f, 0.3f },
        { "CYAN tile = Pick up FLAG NODE",                     0.0f, 1.0f, 1.0f },
        { "PURPLE tile = Deliver FLAG NODE for Points",        0.5f, 0.0f, 1.0f },
    };

    float xPos = -780.0f;
    float startY = 420.0f;
    float stepY = 25.0f;

    float textX, textY;
    for (int i = 0; i < 4; i++) {
        GetNormalizedTextPosition(xPos, startY - i * stepY, &textX, &textY);
        AEGfxPrint(font, hints[i].text, textX, textY, 0.85f,
            hints[i].r, hints[i].g, hints[i].b, 1.00f);
    }
}
void UpdateAndRenderGameplay(
    Player& player,
    float dt,
    std::vector<std::vector<int>>* gameMap,
    CameraPos& camera,
    std::vector<Bullet>& playerBullets,
    std::vector<Bullet>& enemyBullets,
    std::vector<Grunt>& grunts,
    EventManager& events)
{
	WeaponStats weapon = GetCurrentWeapon(player);
    ShowCursor(false); 

    Background_Draw(camera);
    Load_Map(*gameMap,camera, false);

    UpdatePlayer(&player, dt, *gameMap);
    WeaponSwitchCheck(player);

    if (player.currentWeaponIndex == 2)
        SpreadShootingCheck(player, playerBullets, weapon, camera, 5, 30.0f);
    else
        ShootingCheck(player, playerBullets, weapon, camera);   
    ReloadBullets(weapon, dt);

    UpdateGrunts(grunts, player, enemyBullets, dt, *gameMap);
    UpdateGruntSpawner(grunts, *gameMap, dt);

    UpdateBullets(playerBullets, dt, *gameMap);
    UpdateBullets(enemyBullets, dt, *gameMap);
    UpdateDamageNumbers(dt);

    CheckBulletGruntCollisions(playerBullets, grunts, player);


    CheckEnemyBulletPlayerCollision(enemyBullets, player);
   
    CheckPlayerGruntBodyCollision(player, grunts);

    RenderGrunts(grunts, meshes.circle, camera);
    DrawPlayer(player, meshes.circle,meshes.arrow, camera, dt, *gameMap);
    RenderBullets(player,playerBullets, meshes.circle,camera);
    RenderBullets(enemyBullets, meshes.circle, camera);
    DrawPlayerUI(player, weapon, meshes.square, meshes.circle, camera);
    DrawCrosshair(meshes.square);
    RenderDamageNumbers(camera);

    CheckWinCondition(events, player);
    RemoveDeadGrunts(grunts);



    DrawScoreUI(player, meshes.square, dt);

}