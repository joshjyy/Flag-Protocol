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
#include "powerup.hpp"

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
    Activate_Firewall(playerPowerUps, meshes.circle, dt, player, camera);
    if (AEInputCheckTriggered(AEVK_F)) {
        playerPowerUps.firewall = true;
        playerPowerUps.firewallTimer = 5.0f;
    }

    CheckEnemyBulletPlayerCollision(enemyBullets, player, playerPowerUps);
   
    CheckPlayerGruntBodyCollision(player, grunts);

    RenderGrunts(grunts, meshes.circle, camera);
    DrawPlayer(player, meshes.circle, camera, dt);
    RenderBullets(player,playerBullets, meshes.circle,camera);
    RenderBullets(enemyBullets, meshes.circle, camera);
    DrawPlayerUI(player, weapon, meshes.square, meshes.circle, camera);
    DrawCrosshair(meshes.square);
    RenderDamageNumbers(camera);

    CheckWinCondition(events, player);
    RemoveDeadGrunts(grunts);



    DrawScoreUI(player, meshes.square, dt);
}