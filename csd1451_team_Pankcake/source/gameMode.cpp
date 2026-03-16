#include "gameMode.hpp"
#include "utils.hpp"
#include "enemies.hpp"
#include "player.hpp"
#include "damageNumbers.hpp"
#include "AI.hpp"
#include "weapon.hpp"
#include "events.hpp"
#include "map.hpp"


void UpdateAndRenderGameplay(
    Player& player,
    float dt,
    std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>* gameMap,
    float& cameraX, float& cameraY,
    std::vector<Bullet>& playerBullets,
    std::vector<Bullet>& enemyBullets,
    std::vector<Grunt>& grunts,
    EventManager& events)
{
	WeaponStats weapon = GetCurrentWeapon(player);

    cameraX = player.x;
    cameraY = player.y;
    Load_Map(*gameMap, cameraX, cameraY);

    UpdatePlayer(&player, dt, *gameMap);
    WeaponSwitchCheck(player);
    ShootingCheck(player, playerBullets, weapon, cameraX, cameraY);
    ReloadBullets(weapon, dt);

    UpdateGrunts(grunts, player, enemyBullets, dt, *gameMap);
    UpdateGruntSpawner(grunts, *gameMap, dt);

    UpdateBullets(playerBullets, dt, *gameMap);
    UpdateBullets(enemyBullets, dt, *gameMap);
    UpdateDamageNumbers(dt);

    CheckBulletGruntCollisions(playerBullets, grunts, player);
    CheckEnemyBulletPlayerCollision(enemyBullets, player);
    CheckPlayerGruntBodyCollision(player, grunts);

    RenderGrunts(grunts, meshes.circle, cameraX, cameraY);
    DrawPlayer(player, meshes.circle, cameraX, cameraY, dt);
    RenderBullets(playerBullets, meshes.circle, cameraX, cameraY);
    RenderBullets(enemyBullets, meshes.circle, cameraX, cameraY);
    DrawPlayerUI(player, weapon, meshes.square, meshes.circle, cameraX, cameraY);
    DrawCrosshair(meshes.square);
    RenderDamageNumbers(cameraX, cameraY);

    RemoveDeadGrunts(grunts);
    CheckWinCondition(events, player);
    DrawScoreUI(player, meshes.square, dt);
}