#pragma once


#include "gunTypes.hpp"
#include <vector>
#include "map.hpp"
#include "particle.hpp"
// Bullet structure
struct Bullet {
    float x, y;
    float velocityX, velocityY;
    float damage;
    bool active;
};

struct Player {
	int gruntsKilled;
    float health;
    // Position
    float x;
    float y;

    // Dash system
    float dashTimer;
    float dashCooldownTimer;
    float dashDirX;
    float dashDirY;

    // Weapon system
    Pistol pistol;
    AssaultRifle rifle;
    int currentWeaponIndex;  // 0 = pistol, 1 = rifle
    float shootCooldown;

    bool inDash;
	bool isDead;

    float velocityX;
    float velocityY;
    float prevX;
    float prevY;
    AEGfxVertexList* particleMesh;
    ParticleEmitter* trailEmitter;
    ParticleEmitter* deathEmitter;
    bool deathExplosionSpawned;

};

Bullet CreateBullet(float x, float y, float targetX, float targetY, float speed, float damage);
Player InitPlayer(void);
void InitPlayerParticles(Player* player, void* mesh);

void DrawPlayer(Player& player, void* mesh, float cameraX, float cameraY, float dt);
void DrawPlayerUI(Player& player, WeaponStats& weapon, void* mesh, void* mesh2, float cameraX, float cameraY);  
void PlayerDashStaminaBar(Player& player, void* mesh, float cameraX, float cameraY);
void NormalizeVector(float* x, float* y);
void UpdateBullets(std::vector<Bullet>& bullets, float dt, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);
void UpdateBullets(std::vector<Bullet>& bullets, float dt); // Overload for enemie bullets
void RenderBullets(const std::vector<Bullet>& bullets, void* mesh, float cameraX, float cameraY);
void ClearBullets(std::vector<Bullet>& bullets);  
int GetBulletCount(const std::vector<Bullet>& bullets);
WeaponStats GetCurrentWeapon(Player& player);
void UpdatePlayer(Player* player, float dt, std::array<std::array<int, MAP_WIDTH>, MAP_HEIGHT>& gameMap);


void CheckEnemyBulletPlayerCollision(std::vector<Bullet>& enemyBullets, Player& player);

