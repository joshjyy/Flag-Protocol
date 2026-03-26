#pragma once


#include "gunTypes.hpp"
#include <vector>
#include "map.hpp"
#include "particle.hpp"

struct Grunt;
// Bullet structure
struct Bullet {
    float x, y;
    float velocityX, velocityY;
    float damage;
    bool active;
};

struct Player {
	int gruntsKilled;
	int flagCaptures;
	int score;
    int scoreMilestone;
    float milestoneTimer;
    
    float health;

    bool canshoot;
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
    Shotgun shotgun;
    int currentWeaponIndex;  // 0 = pistol, 1 = rifle
    float shootCooldown;

    bool inDash;
	bool isDead;
	bool holdingFlag;
    bool wasSprinting;
	bool lowHealthWarningPlayed;

    float velocityX;
    float velocityY;
    float prevX;
    float prevY;


    AEGfxVertexList* particleMesh;
    ParticleEmitter* trailEmitter;
    ParticleEmitter* deathEmitter;
    ParticleEmitter* dashTrailEmitter; 
    bool deathExplosionSpawned;



};

Bullet CreateBullet(float x, float y, float targetX, float targetY, float speed, float damage);
Player InitPlayer(void);
void InitPlayerParticles(Player* player, void* mesh, void* mesh2);

void DrawPlayer(Player& player, void* mesh, CameraPos& camera, float dt);
void DrawPlayerUI(Player& player, WeaponStats& weapon, void* mesh, void* mesh2, CameraPos camera);
void PlayerDashStaminaBar(Player& player, void* mesh, CameraPos& camera);
void UpdateBullets(std::vector<Bullet>& bullets, float dt, std::vector<std::vector<int>>& gameMap);
void UpdateBullets(std::vector<Bullet>& bullets, float dt); // Overload for enemie bullets
//Overload for Player
void RenderBullets(Player& player,const std::vector<Bullet>& bullets, void* mesh, CameraPos camera);

void RenderBullets(const std::vector<Bullet>& bullets, void* mesh, CameraPos camera);
void ClearBullets(std::vector<Bullet>& bullets);  
int GetBulletCount(const std::vector<Bullet>& bullets);
WeaponStats GetCurrentWeapon(Player& player);
void UpdatePlayer(Player* player, float dt, std::vector<std::vector<int>>& gameMap);
void DrawCrosshair(void* mesh);


void CheckEnemyBulletPlayerCollision(std::vector<Bullet>& enemyBullets, Player& player);

void CheckPlayerGruntBodyCollision(Player& player, std::vector<Grunt>& grunts);
void killPlayer(Player& player);
void LoadPlayer(Player& player);
void FreePlayer(Player& player);

void DrawScoreUI(Player& player, void* mesh, float dt);
void SavePlayerScore(Player& player);

bool CanShoot(Player& player);
